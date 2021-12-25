#include <volt/pch.hpp>
#include <volt/gpu/sampler.hpp>

namespace volt::gpu {

sampler::~sampler() {
	destroy();
};

const std::shared_ptr<gpu::device> &sampler::device() {
	return _device;
}

texture_filter sampler::filter() {
	return _filter;
}

void sampler::filter(texture_filter filter) {
	if (filter != _filter) {
		_filter = filter;
		destroy();
		create();
	}
}

bool sampler::blur() {
	return _blur;
}

void sampler::blur(bool blur) {
	if (blur != _blur) {
		_blur = blur;
		destroy();
		create();
	}
}

float sampler::anisotropy() {
	return _anisotropy;
}

void sampler::anisotropy(float anisotropy) {
	if (anisotropy != _anisotropy) {
		_anisotropy = anisotropy;
		destroy();
		create();
	}
}

sampler::sampler(std::shared_ptr<gpu::device> &&device, texture_filter filter, bool blur, float anisotropy)
		: _device(std::move(device)), _filter(filter), _blur(blur), _anisotropy(anisotropy) {
	create();
}

}
