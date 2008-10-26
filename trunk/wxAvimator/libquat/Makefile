CFLAGS += -I. -Wno-deprecated

all: MEM_RefCountedC-Api.o MT_Assert.o MT_CmMatrix4x4.o MT_ExpMap.o MT_Matrix3x3.o MT_Matrix4x4.o MT_Plane3.o MT_Point3.o MT_Quaternion.o MT_Transform.o MT_Vector2.o MT_Vector3.o MT_Vector4.o MT_random.o
	ar r libquat.a *.o
	ranlib libquat.a

.cpp.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *.a *~
