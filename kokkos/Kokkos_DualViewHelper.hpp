#pragma once

#include <Kokkos_DualView.hpp>

template <typename DV>
auto d_view(const DV &dv) {
    return dv.template view<typename DV::t_dev::memory_space>();
}

template <typename DV>
auto h_view(const DV &dv) {
    return dv.template view<typename DV::t_host::memory_space>();
}