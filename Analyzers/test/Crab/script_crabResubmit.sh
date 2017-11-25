#!/bin/bash

index=1
while [ ${index} -le 40 ]; do

  echo "`date`";

  python crabStatusResubmit.py resubmit >&resubmit.log&

  index=$((index+1))
  sleep 1800;
done;

python crabStatusResubmit.py status >&status.log&
