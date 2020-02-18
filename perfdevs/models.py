import pickle
import logging
from abc import ABC, abstractmethod
from collections import deque, defaultdict
from typing import Any, Iterator, Tuple, List, Dict

from . import PHASE_ACTIVE, PHASE_PASSIVE, INFINITY

logging.basicConfig(level=logging.DEBUG)

PORT_IN = "in"
PORT_OUT = "out"


class Port:
    def __init__(self, p_type=None, name: str = None, serve: bool = False):
        """
        xDEVS implementation of DEVS Port.
        :param p_type: data type of messages to be sent/received via the new port instance.
        :param name: name of the new port instance.
        :param serve: set to True if the port is going to be accessible via RPC server.
        """
        self.name = name if name else self.__class__.__name__
        self.parent = None
        self.direction = None       # added direction to ports
        self.values = deque()
        self.p_type = p_type
        self.serve = serve
        self.links_to = list()      # List of ports that receive data from the port (only used by output ports)
        self.links_from = [self]    # List of ports that inject data to the port (only used by input ports)

    def __bool__(self) -> bool:
        return any((bool(port.values) for port in self.links_from))

    def __len__(self) -> int:
        return sum((len(port.values) for port in self.links_from))

    def __str__(self) -> str:
        return "{Port(%s): [%s]}" % (self.p_type, list(self.get_all()))

    def empty(self) -> bool:
        return all((not port for port in self.links_from))

    def clear(self):
        self.values.clear()

    def get(self) -> Any:
        """
        :return: ONE single message from port.
            By default, it will return a value contained in its own values bag.
            If no message is found in its own bag, it will iterate over chained ports looking for a message.
        :raises IndexError: if port is empty.
        """
        for port in self.links_from:
            try:
                return port.values[0]
            except IndexError:
                continue
        raise IndexError

    def get_all(self) -> Iterator[Any]:
        """:return: iterator containing all the messages in the port."""
        for port in self.links_from:
            for val in port.values:
                yield val

    def add(self, val: Any):
        """
        Adds a new value to the local message bag of the port.
        :param val: message to be added.
        :raises TypeError: If message is not instance of port type.
        """
        if type and not isinstance(val, self.p_type):
            raise TypeError("Value type is %s (%s expected)" % (type(val).__name__, self.p_type.__name__))
        self.values.append(val)

    def extend(self, vals: Iterator[Any]):
        """
        Adds a set of new values to the local message bag of the port.
        :param vals: list containing all the messages to be added.
        :raises TypeError: If one of the messages is not instance of port type.
        """
        for val in vals:
            self.add(val)


class Component(ABC):
    def __init__(self, name: str = None):
        """
        Abstract Base Class for an xDEVS model.
        :param name: name of the xDEVS model. If no name is provided, it will take the class's name by default.
        """
        self.name = name if name else self.__class__.__name__
        self.chain = False  # True if component is a chain
        self.link = False   # True if component is a link of a chain
        self.parent = None
        self.in_ports = list()
        self.out_ports = list()

    def __str__(self) -> str:
        """:return: stringified version of a component"""
        in_str = " ".join([p.name for p in self.in_ports])
        out_str = " ".join([p.name for p in self.out_ports])
        return "%s: InPorts[%s] OutPorts[%s]" % (self.name, in_str, out_str)

    @abstractmethod
    def initialize(self):
        """Initializes the xDEVS model."""
        pass

    @abstractmethod
    def exit(self):
        """Exits the xDEVS model."""
        pass

    def in_empty(self) -> bool:
        """:return: True if model has not any message in all its input ports."""
        return all([p.empty() for p in self.in_ports])

    def out_empty(self) -> bool:
        """:return: True if model has not any message in all its output ports."""
        return all([p.empty() for p in self.out_ports])

    def add_in_port(self, port: Port):
        """
        Adds an input port to the xDEVS model.
        :param port: port to be added to the model.
        """
        port.parent = self
        port.direction = PORT_IN
        self.in_ports.append(port)

    def add_out_port(self, port: Port):
        """
        Adds an output port to the xDEVS model
        :param port: port to be added to the model.
        """
        port.parent = self
        port.direction = PORT_OUT
        self.out_ports.append(port)


class Coupling:
    def __init__(self, port_from: Port, port_to: Port, host=None):  # TODO identify host's variable type
        """
        xDEVS implementation of DEVS couplings.
        :param port_from: DEVS transmitter port.
        :param port_to: DEVS receiver port.
        :param host: TODO documentation for this
        :raises ValueError: if coupling direction is incompatible with the target ports.
        """
        # Check that couplings are valid
        comp_from = port_from.parent
        comp_to = port_to.parent
        if isinstance(comp_from, Atomic) and port_from.direction == PORT_IN:
            raise ValueError("Input ports whose parent is an Atomic model can not be coupled to any other port")
        if isinstance(comp_to, Atomic) and port_to.direction == PORT_OUT:
            raise ValueError("Output ports whose parent is an Atomic model can not be recipient of any other port")
        # TODO assert that port types are compatible
        self.port_from = port_from
        self.port_to = port_to
        self.host = host

    def __str__(self) -> str:
        """:return: stringified version of the coupling."""
        return "(%s -> %s)" % (self.port_from, self.port_to)

    def propagate(self):
        """Copies messages from the transmitter port to the receiver port"""
        if self.host:
            if len(self.port_from) > 0:
                values = list(map(lambda x: pickle.dumps(x, protocol=0).decode(), self.port_from.get_all()))
                try:
                    self.host.inject(self.port_to, values)
                except:  # TODO identify exception type
                    logging.warning("Values could not be injected (%s)" % self.port_to)
        else:
            self.port_to.extend(self.port_from.get_all())


class Atomic(Component, ABC):
    def __init__(self, name: str = None):
        """
        xDEVS implementation of DEVS Atomic Model.
        :param name: name of the Atomic Model. If no name is provided, it will take the class's name by default.
        """
        self.name = name if name else self.__class__.__name__
        super().__init__(name)

        self.phase = PHASE_PASSIVE
        self.sigma = INFINITY

    @property
    def ta(self) -> Any:
        """:return: remaining time for the atomic model's internal transition."""
        return self.sigma

    def __str__(self) -> str:
        """:return: stringified representation of atomic model."""
        return "%s(%s, %s)" % (self.name, self.phase, self.sigma)

    @abstractmethod
    def deltint(self):
        """Describes the internal transitions of the atomic model."""
        pass

    @abstractmethod
    def deltext(self, e: Any):
        """
        Describes the external transitions of the atomic model.
        :param e: elapsed time between last transition and the external transition.
        """
        pass

    @abstractmethod
    def lambdaf(self):
        """Describes the output function of the atomic model."""
        pass

    def deltcon(self, e: Any):
        """
        Describes the confluent transitions of the atomic model. By default, the internal transition is triggered first.
        :param e: elapsed time between last transition and the confluent transition.
        """
        self.deltint()
        self.deltext(0)

    def hold_in(self, phase: Any, sigma: Any):
        """
        Change atomic model's phase and next timeout.
        :param phase: atomic model's new phase.
        :param sigma: time remaining to the next timeout.
        """
        self.phase = phase
        self.sigma = sigma

    def activate(self):
        """Sets phase to PHASE_ACTIVE and next timeout to 0"""
        self.phase = PHASE_ACTIVE
        self.sigma = 0

    def passivate(self, phase=PHASE_PASSIVE):
        """Sets phase to PHASE_PASSIVE and next timeout to INFINITY"""
        self.phase = phase
        self.sigma = INFINITY


class Coupled(Component, ABC):
    def __init__(self, name: str = None, chain_en: bool = False):
        """
        xDEVS implementation of DEVS Coupled Model.
        :param name: name of the Atomic Model. If no name is provided, it will take the class's name by default.
        :param chain_en: set it to True to enable submodel chaining. By default, chaining is disabled.
        """
        self.name = name if name else self.__class__.__name__
        super().__init__(name)
        self.chain = chain_en

        self.components = []
        self.ic = []
        self.eic = []
        self.eoc = []

    def initialize(self):
        pass

    def exit(self):
        pass

    def add_coupling(self, p_from: Port, p_to: Port, host=None):
        """
        Adds coupling betweem two submodules of the coupled model.
        :param p_from: DEVS transmitter port.
        :param p_to: DEVS receiver port.
        :param host: TODO documentation
        :raises ValueError: if coupling is not well defined.
        """
        if self.chain and host is not None:
            raise ValueError("A chain cannot host couplings")
        coupling = Coupling(p_from, p_to, host)
        if p_from.parent == self and p_to.parent in self.components:
            self.eic.append(coupling)
        elif p_from.parent in self.components and p_to.parent == self:
            self.eoc.append(coupling)
        elif p_from.parent in self.components and p_to.parent in self.components:
            self.ic.append(coupling)
        else:
            raise ValueError("Components that compose the coupling are not submodules of coupled model")

    def remove_coupling(self, coupling: Coupling):
        """
        Removes coupling betweem two submodules of the coupled model.
        :param coupling: Couplings to be removed.
        :raises ValueError: if coupling is not found.
        """
        if coupling in self.eic:
            self.eic.remove(coupling)
        elif coupling in self.eoc:
            self.eoc.remove(coupling)
        elif coupling in self.ic:
            self.ic.remove(coupling)
        else:
            raise ValueError("Coupling was not found in model definition")

    def add_component(self, component: Component):
        """
        Adds subcomponent to coupled model.
        :param component: component to be added to the Coupled model.
        """
        component.parent = self
        self.components.append(component)

    def chain_components(self, force: bool = False, split: bool = False) -> Tuple[List, List, Dict[Port, List[Port]]]:
        """
        Chains submodules to enhance sequential execution performance.
        :param force: if True, the entire hierarchy is chained. By default, it is set to False.
        :param split: if True, coupled models may be divided into an equivalent set of chained and unchained models.
        :return: True if chaining was triggered.
        """
        self.chain |= force
        comps_up = list()           # It will contain children coupled models to be splitted from the chain
        new_ics_up = list()         # It will contain internal couplings to be added to the parent due to splits
        ports_split_up = dict()     # Legacy ports are keys. List of new ports are values

        self._resolve_subchain_splits(force, split)  # If subcomponents are split chains, resolve new model topology

        if self.chain:
            if split and self.parent is not None:
                self._trigger_split(comps_up, new_ics_up, ports_split_up)  # Split internal coupled models
            self._trigger_chaining()        # Remaining components are then chained between them
            self._unroll_internal_chains()  # Chains within chains are unrolled

        return comps_up, new_ics_up, ports_split_up

    def _resolve_subchain_splits(self, force, split):
        comps_down = list()         # It will contain new children coupled models to be added due to splits
        new_ics_down = list()       # It will contain new internal couplings to be added due to splits
        ports_split_down = dict()   # It will contain port split of internal chains due to splits

        # First, we trigger chaining to all the children models and gather all the splits together
        for comp in self.components:
            if isinstance(comp, Coupled):
                new_comps, new_ics, new_ports = comp.chain_components(force, split)
                comps_down.extend(new_comps)
                new_ics_down.extend(new_ics)
                ports_split_down.update(new_ports)
        # All new components are added to the hierarchy
        for comp in comps_down:
            self.add_component(comp)
        # External ports of children could be split. We build new couplings and remove legacy couplings
        for prev_port, new_ports in ports_split_down.values():
            prev_coups = [coup for coup in self.eic if coup.port_to == prev_port]
            prev_coups.extend([coup for coup in self.eoc if coup.port_from == prev_port])
            prev_coups.extend([coup for coup in self.ic if prev_port in [coup.port_from, coup.port_to]])

            new_coups = list()
            for prev_coup in prev_coups:
                for new_port in new_ports:
                    if prev_coup.port_to == prev_port:
                        new_coups.append(Coupling(prev_coup.port_from, new_port))
                    elif prev_coup.port_from == prev_port:
                        new_coups.append(Coupling(new_port, prev_coup.port_to))

            for coup in prev_coups:
                self.remove_coupling(coup)
            for coup in new_coups:
                self.add_coupling(coup.port_from, coup.port_to)

        for coup in new_ics_down:
            self.add_coupling(coup.port_from, coup.port_to)

    def _trigger_split(self, comps_up, new_ics_up, ports_split_up):
        for comp in self.components:
            if isinstance(comp, Coupled) and not comp.chain:
                int_ports_splits = dict()  # {comp_port: new_ext_port}
                new_ics_up.extend(self.__split(comp, ports_split_up, int_ports_splits))
                comps_up.append(comp)

        remaining_operating_ports = [coup.port_from for coup in self.eic]
        remaining_operating_ports.extend([coup.port_to for coup in self.eoc])
        for port in ports_split_up:
            if port in remaining_operating_ports:
                ports_split_up[port].append(port)
            else:
                try:
                    self.in_ports.remove(port)
                except ValueError:
                    self.out_ports.remove(port)

        for comp in comps_up:
            self.components.remove(comp)

    def __split(self, comp, ext_ports_split, int_ports_split) -> List:
        assert isinstance(comp, Coupled)

        # FIRST EXTERNAL COUPLINGS ARE DELETED AND PORTS SPLIT
        eic_to = [coup for coup in self.eic if coup.port_to in comp.in_ports]
        eoc_from = [coup for coup in self.eoc if coup.port_from in comp.out_ports]
        ic_to = [coup for coup in self.ic if coup.port_to in comp.in_ports]
        ic_from = [coup for coup in self.ic if coup.port_from in comp.out_ports]

        for coup in eic_to:
            if coup.port_from not in ext_ports_split:
                ext_ports_split[coup.port_from] = list()
            ext_ports_split[coup.port_from].append(coup.port_to)
            self.remove_coupling(coup)
        for coup in eoc_from:
            if coup.port_to not in ext_ports_split:
                ext_ports_split[coup.port_to] = list()
            ext_ports_split[coup.port_to].append(coup.port_from)
            self.remove_coupling(coup)

        ics_up = list()
        for coup in ic_to:
            if coup.port_to not in int_ports_split:
                new_port = Port(coup.port_to.p_type)
                self.add_out_port(new_port)
                int_ports_split[coup.port_to] = new_port
            self.add_coupling(coup.port_from, int_ports_split[coup.port_to])
            ics_up.append(Coupling(int_ports_split[coup.port_to], coup.port_to))
            self.remove_coupling(coup)
        for coup in ic_from:
            if coup.port_from not in int_ports_split:
                new_port = Port(coup.port_from.p_type)
                self.add_in_port(new_port)
                int_ports_split[coup.port_from] = new_port
            self.add_coupling(int_ports_split[coup.port_from], coup.port_to)
            ics_up.append(Coupling(int_ports_split[coup.port_from], coup.port_from))
            self.remove_coupling(coup)

        return ics_up

    def _trigger_chaining(self):
        for comp in self.components:
            comp.link = self.chain
        for coup in self.eic:
            chain_from_coupling(coup)
        self.eic.clear()
        for coup in self.eoc:
            chain_from_coupling(coup)
        self.eoc.clear()
        for coup in self.ic:
            chain_from_coupling(coup)
        self.ic.clear()

    def _unroll_internal_chains(self):
        # Las cadenas internas las desenrollo
        new_comps = list()
        prev_comps = list()
        for comp in self.components:
            if isinstance(comp, Coupled) and comp.chain:
                unroll_chain(comp)
                prev_comps.append(comp)
                new_comps.extend(comp.components)
        for comp in new_comps:
            self.add_component(comp)
        for comp in prev_comps:
            self.components.remove(comp)

    def flatten(self) -> Tuple[List, List]:
        """
        Flattens coupled model (i.e., parent coupled model inherits the connection of the model).
        :return: Components and couplings to be transferred to parent
        """
        new_comp = list()   # list with children components to be inherited by parent
        new_coup = list()   # list with couplings to be inherited by parent

        if not self.chain:  # Chains cannot be flattened
            old_comp = list()       # list with children coupled models to be deleted
            inherit_comp = list()   # list with components to be inherited from children
            inherit_coup = list()

            for comp in self.components:
                if isinstance(comp, Coupled) and not comp.chain:  # Propagate flattening to children coupled models
                    comps, coup = comp.flatten()
                    old_comp.append(comp)
                    inherit_comp.extend(comps)
                    inherit_coup.extend(coup)

            for comp in old_comp:
                self._remove_child_couplings(comp)
                self.components.remove(comp)

            for comp in inherit_comp:
                self.add_component(comp)
            for coup in inherit_coup:
                self.add_coupling(coup.port_from, coup.port_to)

            if self.parent is not None:  # If module is not root, trigger the flatten process
                new_comp.extend(self.components)

                left_bridge_eic = self._create_left_bridge(self.parent.eic)
                new_coup.extend(self._complete_left_bridge(left_bridge_eic))

                left_bridge_ic = self._create_left_bridge(self.parent.ic)
                right_bridge_ic = self._create_right_bridge(self.parent.ic)
                new_coup.extend(self._complete_left_bridge(left_bridge_ic))
                new_coup.extend(self._complete_right_bridge(right_bridge_ic))

                right_bridge_eoc = self._create_right_bridge(self.parent.eoc)
                new_coup.extend(self._complete_right_bridge(right_bridge_eoc))

                new_coup.extend(self.ic)

        return new_comp, new_coup

    def _remove_child_couplings(self, child):
        for in_port in child.in_ports:
            self.__remove_couplings(in_port, self.eic)
            self.__remove_couplings(in_port, self.ic)
        for out_port in child.out_ports:
            self.__remove_couplings(out_port, self.ic)
            self.__remove_couplings(out_port, self.eoc)

    @staticmethod
    def __remove_couplings(port, couplings):
        to_remove = list()
        for coup in couplings:
            if coup.port_to == port or coup.port_from == port:
                to_remove.append(coup)
        for coup in to_remove:
            couplings.remove(coup)

    def _create_left_bridge(self, pc):
        bridge = defaultdict(list)
        for in_port in self.in_ports:
            for coup in pc:
                if coup.port_to == in_port:
                    bridge[in_port].append(coup.port_from)
        return bridge

    def _create_right_bridge(self, pc):
        bridge = defaultdict(list)
        for out_port in self.out_ports:
            for coup in pc:
                if coup.port_from == out_port:
                    bridge[out_port].append(coup.port_to)
        return bridge

    def _complete_left_bridge(self, bridge):
        couplings = list()
        for coup in self.eic:
            ports = bridge[coup.port_from]
            for port in ports:
                couplings.append(Coupling(port, coup.port_to))
        return couplings

    def _complete_right_bridge(self, bridge):
        couplings = list()
        for coup in self.eoc:
            ports = bridge[coup.port_to]
            for port in ports:
                couplings.append(Coupling(coup.port_from, port))
        return couplings


def chain_from_coupling(coup: Coupling):
    coup.port_from.links_to.append(coup.port_to)
    coup.port_to.links_from.append(coup.port_from)


def unroll_chain(comp: Coupled):
    assert comp.chain and not comp.eic and not comp.eoc and not comp.ic

    for in_port in comp.in_ports:
        for port in in_port.couplings_to:
            port.couplings_from.remove(in_port)
            port.couplings_from.extend(in_port.couplings_from)
        for port in in_port.couplings_from:
            port.couplings_to.remove(in_port)
            port.couplings_to.extend(in_port.couplings_to)

    for out_port in comp.out_ports:
        for port in out_port.couplings_from:
            port.couplings_to.remove(out_port)
            port.couplings_to.extend(out_port.couplings_to)
        for port in out_port.couplings_to:
            port.couplings_from.remove(out_port)
            port.couplings_from.extend(out_port.couplings_from)
