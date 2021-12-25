#include <volt/gpu/gl46/instance.hpp>

#include <volt/gpu/gl46/gl46.hpp>
#include <volt/log.hpp>

namespace volt::gpu::gl46 {

void GLAPIENTRY debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
		GLsizei length, const GLchar *message, const void* userParam) {
	std::string msg = "[OpenGL] ";
	msg += message;

	if (severity == GL_DEBUG_SEVERITY_HIGH)
		VOLT_LOG_ERROR(msg)
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
		VOLT_LOG_WARNING(msg)
	else
		VOLT_LOG_INFO(msg)
}

std::vector<std::shared_ptr<gpu::adapter>> instance::enumerate_adapters() {
	return {}; // Return single default adapter
}

uint32_t instance::concurrent_frames() {
	return gl46::concurrent_frames;
}

}
