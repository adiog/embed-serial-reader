#!/bin/bash

DEVICE=$1
REFRESH_RATE=${2:-0.05}
X_AXIS_POINTS=${3:-20}

DEVICE_PATH=/dev/shm/${DEVICE}

SCREEN_BASE=SCREEN-MPU-${DEVICE}

rm -fr ${DEVICE_PATH}

mkdir -p ${DEVICE_PATH}

screen -ls | grep ${SCREEN_BASE} | awk '{print "screen -S "$1" -X quit";}' | tee | bash

for var in acc gyro mag;
do
    VAR_BASE=${DEVICE_PATH}/${var}
    VAR_LOOP=${VAR_BASE}.loop

    case ${var} in
        "acc" )
            PLOT_TITLE="${DEVICE} Accelerometer"
            PLOT_UNITS="m/s^2"
            Y_AXIS_MIN="-20"
            Y_AXIS_MAX="20"
            ;;
        "gyro" )
            PLOT_TITLE="${DEVICE} Gyroscope"
            PLOT_UNITS="rads"
            Y_AXIS_MIN="-2"
            Y_AXIS_MAX="2"
            ;;
        "mag" )
            PLOT_TITLE="${DEVICE} Magnetometer"
            PLOT_UNITS="uT"
            Y_AXIS_MIN="-150"
            Y_AXIS_MAX="150"
            ;;
    esac

    (
    echo "set terminal x11 noraise"
    echo ""
    echo "set title \"${PLOT_TITLE}\""
    echo "set xrange [0:${X_AXIS_POINTS}]"
    echo "set yrange [${Y_AXIS_MIN}:${Y_AXIS_MAX}]"
    echo "plot \\"
    ) > ${VAR_LOOP}

    for axis in X Y Z;
    do
        VAR_PIPE=${VAR_BASE}${axis}
        mkfifo ${VAR_PIPE}
        VAR_PLOT=${VAR_PIPE}.plot
        touch ${VAR_PLOT}
        screen -d -m -S ${SCREEN_BASE}-${var}-${axis} python plotter.py ${X_AXIS_POINTS} ${VAR_PIPE} ${VAR_PLOT}

        case ${axis} in
            "X" )
                PLOT_COLOR="red" ;;
            "Y" )
                PLOT_COLOR="green" ;;
            "Z" )
                PLOT_COLOR="blue" ;;
        esac

        (
        echo -n "\"${VAR_PLOT}\" using 1:2 with lines linecolor \"${PLOT_COLOR}\" title \"${axis}-axis (${PLOT_UNITS})\""
        [[ "${axis}" = "Z" ]]  \
            && echo ""         \
            || echo ", \\"
        ) >> ${VAR_LOOP}
    done

    (
    echo "pause ${REFRESH_RATE}"
    echo "reread"
    ) >> ${VAR_LOOP}

    screen -d -m -S ${SCREEN_BASE}-${var}-plot gnuplot ${VAR_LOOP}
done
