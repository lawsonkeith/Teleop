# You need to "sudo apt-get install libgtkmm-3.0-dev" to build the demo_3d binary

all: demo_raw 

HDRS = I2Cdev.h 
CMN_OBJS = I2Cdev.o MPU6050.o
RAW_OBJS = demo_raw.o


# Set DMP FIFO rate to 20Hz to avoid overflows on 3d demo.  See comments in
# MPU6050_6Axis_MotionApps20.h for details.

CXXFLAGS = -DDMP_FIFO_RATE=9 -Wall -g -O2 `pkg-config gtkmm-3.0 --cflags --libs`

$(CMN_OBJS) $(DMP_OBJS) $(RAW_OBJS) : $(HDRS)

demo_raw: $(CMN_OBJS) $(RAW_OBJS)
	$(CXX) -o $@ $^ -lm

clean:
	rm -f $(CMN_OBJS) $(DMP_OBJS) $(D3D_OBJS) $(RAW_OBJS) demo_raw demo_dmp demo_3d test_3d

