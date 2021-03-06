#pragma once

namespace StreamCompaction {
namespace Efficient {
    void scan(int n, int *odata, const int *idata);

    int compact(int n, int *odata, const int *idata);

	void scan_device(int n, int *data);
}
}
