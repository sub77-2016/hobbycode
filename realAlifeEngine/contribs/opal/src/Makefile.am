

# set the include path found by configure
INCLUDES = -I$(top_srcdir)/src/ODE -I$(top_srcdir)/src/external/quicktest \
	-I$(top_srcdir)/src/external/tinyxml $(all_includes)

# the library search path.
SUBDIRS = external ODE
noinst_LIBRARIES = libopalode.a
libopalode_a_SOURCES = AccelerationSensor.cpp AttractorMotor.cpp Blueprint.cpp \
	BlueprintInstance.cpp BlueprintManager.cpp CollisionEventHandler.cpp GearedMotor.cpp \
	InclineSensor.cpp Joint.cpp Logger.cpp Motor.cpp RaycastSensor.cpp Sensor.cpp \
	ServoMotor.cpp Simulator.cpp Solid.cpp SolidData.cpp Space.cpp SpringMotor.cpp \
	ThrusterMotor.cpp VolumeSensor.cpp Vec3r.cpp Quaternion.cpp VelocityMotor.cpp
bin_PROGRAMS = test_opal
test_opal_SOURCES = testopal.cpp testAccelerationSensor.cpp \
					testRaycastSensor.cpp testSimulator.cpp testVec3r.cpp testPoint3r.cpp testQuaternion.cpp \
					testMatrix44r.cpp testBlueprint.cpp testMath.cpp testVelocityMotor.cpp testFixedJoint.cpp \
					testJoint.cpp testVolumeSensor.cpp testSolid.cpp testMotor.cpp testServoMotor.cpp \
		testBallJoint.cpp testThrusterMotor.cpp
test_opal_LDADD = $(top_builddir)/src/external/tinyxml/libtinyxml.a \
	$(top_builddir)/src/ODE/libodeimpl.a $(top_builddir)/src/libopalode.a -ltinyxml -lode
noinst_HEADERS = Mass.h VelocityMotor.h VelocityMotorData.h \
	MovementEventHandler.h
libopalode_a_LIBADD = $(top_builddir)/src/external/tinyxml/libtinyxml.a\
	$(top_builddir)/src/ODE/libodeimpl.a

