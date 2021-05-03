#ifndef _LONG_H_
#define _LONG_H_

#include "../../core/modeling/Event.h"

class Long : public BaseEvent {
protected:
  long value;
public:
  Long(const long& value);
  Long(const Long& src);
  const Long& operator=(const Long& right);
  virtual ~Long();

  void setValue(const long& value);
  const long& getValue() const;
};

#endif /* SRC_XDEVS_CORE_TEST_EFP_JOB_H_ */
