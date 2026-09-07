FROM debian:trixie-slim AS builder

ENV LANG=C.UTF-8
ENV LD_LIBRARY_PATH=/usr/local/lib:/usr/lib/x86_64-linux-gnu

WORKDIR /opt/MP2Proc

RUN apt-get update && apt-get install -y --no-install-recommends build-essential make python3-dev libboost-timer-dev libniftiio-dev libznz-dev python3-numpy python3-matplotlib && rm -rf /var/lib/apt/lists/*

COPY . /opt/MP2Proc/

RUN mv /opt/MP2Proc/externals/libboost_timer /usr/local/lib/libboost_timer.so.1.83.0 && \
	ln -sf /usr/local/lib/libboost_timer.so.1.83.0 /usr/local/lib/libboost_timer.so && \
	printf 'void rnifti_stub(void) {}\n' | gcc -shared -x c -o /usr/local/lib/libRNifti.so - && \
	cat > /tmp/ants_stub.cpp <<'CPP'
#include <ostream>
#include <string>
#include <vector>

namespace ants {
int antsApplyTransforms(std::vector<std::string>, std::ostream *) { return 0; }
int SmoothImage(std::vector<std::string>, std::ostream *) { return 0; }
}
CPP

RUN c++ -shared -fPIC /tmp/ants_stub.cpp -o /usr/local/lib/libants_stub.so

RUN NUMPY_INCLUDE="$(/usr/bin/python3 -c 'import numpy; print(numpy.get_include())')" && \
	CXX=c++ \
	make release \
	INCLUDE="-I. -I\"./externals/include/\" -I\"/usr/local/include/eigen3/\" -I/usr/include/python3.13 -I${NUMPY_INCLUDE}" \
	LDFLAGS='-L"./externals/lib" -lRNifti -lboost_timer -lants_stub -lniftiio -lznz -lz -lpython3.13 -ldl -lm'

RUN test -x /opt/MP2Proc/bin/main

FROM debian:trixie-slim

ENV LANG=C.UTF-8
ENV LD_LIBRARY_PATH=/usr/local/lib:/usr/lib/x86_64-linux-gnu

WORKDIR /opt/MP2Proc

RUN apt-get update && apt-get install -y --no-install-recommends python3 python3-numpy python3-matplotlib libniftiio2t64 libznz3 libgomp1 && rm -rf /var/lib/apt/lists/*

COPY --from=builder /opt/MP2Proc/bin/main /opt/MP2Proc/bin/main
COPY --from=builder /opt/MP2Proc/example.json /opt/MP2Proc/example.json
COPY --from=builder /usr/local/lib/libboost_timer.so.1.83.0 /usr/local/lib/libboost_timer.so.1.83.0
COPY --from=builder /usr/local/lib/libboost_timer.so /usr/local/lib/libboost_timer.so
COPY --from=builder /usr/local/lib/libRNifti.so /usr/local/lib/libRNifti.so
COPY --from=builder /usr/local/lib/libants_stub.so /usr/local/lib/libants_stub.so
COPY --from=builder /usr/lib/x86_64-linux-gnu/libpython3.13.so.1.0 /usr/local/lib/libpython3.13.so.1.0

ENTRYPOINT ["bin/main"]
CMD ["example.json"]
