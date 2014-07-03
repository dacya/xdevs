/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package mitris.sim.core.lib.atomic.sources;

import mitris.sim.core.atomic.sinks.Console;
import mitris.sim.core.modeling.Atomic;
import mitris.sim.core.modeling.Coupled;
import mitris.sim.core.modeling.Port;
import mitris.sim.core.simulation.Coordinator;

/**
 *
 * @author jlrisco
 */
public class Step extends Atomic {

    public Port<Double> portOut = new Port<Double>();
    protected double initialValue;
    protected double stepTime;
    protected double finalValue;

    public Step(double initialValue, double stepTime, double finalValue) {
        super.addOutPort(portOut);
        this.initialValue = initialValue;
        this.stepTime = stepTime;
        this.finalValue = finalValue;
        super.holdIn("initialValue", 0.0);
    }

    @Override
    public void deltint() {
        if (super.phaseIs("initialValue")) {
            super.holdIn("finalValue", stepTime);
        } else if (super.phaseIs("finalValue")) {
            super.passivate();
        }
    }

    @Override
    public void deltext(double e) {
    }

    @Override
    public void lambda() {
        if (super.phaseIs("initialValue")) {
            portOut.addValue(initialValue);
        } else if (super.phaseIs("finalValue")) {
            portOut.addValue(finalValue);
        }
    }

    public static void main(String[] args) {
        Coupled stepExample = new Coupled();
        Step step = new Step(0, 15, 10);
        stepExample.addComponent(step);
        Console console = new Console();
        stepExample.addComponent(console);
        stepExample.addCoupling(step, step.portOut, console, console.iIn);
        Coordinator coordinator = new Coordinator(stepExample);
        coordinator.simulate(30.0);
    }
}
