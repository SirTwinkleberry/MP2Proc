FROM python:3.12-slim

ENV LANG=C.UTF-8
ENV LD_LIBRARY_PATH=/usr/local/lib

WORKDIR /opt/MP2Proc

RUN apt-get update && apt-get install -y --no-install-recommends build-essential make python3-dev libboost-timer-dev libniftiio-dev libznz-dev && rm -rf /var/lib/apt/lists/*

ADD . /opt/MP2Proc/

RUN mv /opt/MP2Proc/externals/libboost_timer /usr/local/lib/libboost_timer.so.1.83.0

RUN ln -sf /usr/local/lib/libboost_timer.so.1.83.0 /usr/local/lib/libboost_timer.so

RUN printf 'void rnifti_stub(void) {}\n' | gcc -shared -x c -o /usr/local/lib/libRNifti.so -

RUN cat > /tmp/ants_stub.cpp <<'CPP'
#include <ostream>
#include <string>
#include <vector>

namespace ants {
int antsApplyTransforms(std::vector<std::string>, std::ostream *) { return 0; }
int SmoothImage(std::vector<std::string>, std::ostream *) { return 0; }
}
CPP
RUN c++ -shared -fPIC /tmp/ants_stub.cpp -o /usr/local/lib/libants_stub.so

RUN pip install --no-cache-dir numpy==1.26.4 matplotlib==3.8.0

RUN NUMPY_INCLUDE="$(python3 -c 'import numpy; print(numpy.get_include())')" && \
	CXX=c++ \
	make release \
	INCLUDE="-I. -I\"./externals/include/\" -I\"/usr/local/include/eigen3/\" -I/usr/include/python3.13 -I${NUMPY_INCLUDE}" \
	LDFLAGS='-L"./externals/lib" -lRNifti -lboost_timer -lants_stub -lniftiio -lznz -lz -lpython3.13 -ldl -lm'

RUN test -x /opt/MP2Proc/bin/main
