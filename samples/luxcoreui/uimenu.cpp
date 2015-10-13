/***************************************************************************
 * Copyright 1998-2015 by authors (see AUTHORS.txt)                        *
 *                                                                         *
 *   This file is part of LuxRender.                                       *
 *                                                                         *
 * Licensed under the Apache License, Version 2.0 (the "License");         *
 * you may not use this file except in compliance with the License.        *
 * You may obtain a copy of the License at                                 *
 *                                                                         *
 *     http://www.apache.org/licenses/LICENSE-2.0                          *
 *                                                                         *
 * Unless required by applicable law or agreed to in writing, software     *
 * distributed under the License is distributed on an "AS IS" BASIS,       *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
 * See the License for the specific language governing permissions and     *
 * limitations under the License.                                          *
 ***************************************************************************/

#include <imgui.h>
#include <boost/algorithm/string/predicate.hpp>

#include "luxcoreapp.h"

using namespace std;
using namespace luxrays;
using namespace luxcore;

//------------------------------------------------------------------------------
// MenuRendering
//------------------------------------------------------------------------------

void LuxCoreApp::MenuRendering() {
	if (ImGui::MenuItem("Restart", "Space bar")) {
		// Restart rendering
		session->Stop();
		session->Start();
	}
	if (ImGui::MenuItem("Quit", "ESC"))
		glfwSetWindowShouldClose(window, GL_TRUE);
}

//------------------------------------------------------------------------------
// MenuEngine
//------------------------------------------------------------------------------

void LuxCoreApp::MenuEngine() {
	const string currentEngineType = config->GetProperty("renderengine.type").Get<string>();

	if (ImGui::MenuItem("PATHOCL", "1", (currentEngineType == "PATHOCL")))
		SetRenderingEngineType("PATHOCL");
	if (ImGui::MenuItem("LIGHTCPU", "2", (currentEngineType == "LIGHTCPU")))
		SetRenderingEngineType("LIGHTCPU");
	if (ImGui::MenuItem("PATHCPU", "3", (currentEngineType == "PATHCPU")))
		SetRenderingEngineType("PATHCPU");
	if (ImGui::MenuItem("BIDIRCPU", "4", (currentEngineType == "BIDIRCPU")))
		SetRenderingEngineType("BIDIRCPU");
	if (ImGui::MenuItem("BIDIRVMCPU", "5", (currentEngineType == "BIDIRVMCPU")))
		SetRenderingEngineType("BIDIRVMCPU");
#if !defined(LUXRAYS_DISABLE_OPENCL)
	if (ImGui::MenuItem("RTPATHOCL", "6", (currentEngineType == "RTPATHOCL")))
		SetRenderingEngineType("RTPATHOCL");
#endif
	if (ImGui::MenuItem("BIASPATHCPU", "7", (currentEngineType == "BIASPATHCPU"))) {
		SetRenderingEngineType("BIASPATHCPU");
		samplerWindow.opened = false;
	}
	if (ImGui::MenuItem("BIASPATHOCL", "8", (currentEngineType == "BIASPATHOCL"))) {
		SetRenderingEngineType("BIASPATHOCL");
		samplerWindow.opened = false;
	}
#if !defined(LUXRAYS_DISABLE_OPENCL)
	if (ImGui::MenuItem("RTBIASPATHOCL", "9", (currentEngineType == "RTBIASPATHOCL"))) {
		SetRenderingEngineType("RTBIASPATHOCL");
		samplerWindow.opened = false;
	}
#endif
}

//------------------------------------------------------------------------------
// MenuSampler
//------------------------------------------------------------------------------

void LuxCoreApp::MenuSampler() {
	const string currentSamplerType = config->GetProperty("sampler.type").Get<string>();

	if (ImGui::MenuItem("RANDOM", NULL, (currentSamplerType == "RANDOM")))
		EditRenderConfig(Properties() << Property("sampler.type")("RANDOM"));
	if (ImGui::MenuItem("SOBOL", NULL, (currentSamplerType == "SOBOL")))
		EditRenderConfig(Properties() << Property("sampler.type")("SOBOL"));
	if (ImGui::MenuItem("METROPOLIS", NULL, (currentSamplerType == "METROPOLIS")))
		EditRenderConfig(Properties() << Property("sampler.type")("METROPOLIS"));
}

//------------------------------------------------------------------------------
// MenuFilm
//------------------------------------------------------------------------------

void LuxCoreApp::MenuFilm() {
	if (ImGui::BeginMenu("Set resolution")) {
		if (ImGui::MenuItem("320x240"))
			SetFilmResolution(320, 240);
		if (ImGui::MenuItem("640x480"))
			SetFilmResolution(640, 480);
		if (ImGui::MenuItem("800x600"))
			SetFilmResolution(800, 600);
		if (ImGui::MenuItem("1024x768"))
			SetFilmResolution(1024, 768);
		if (ImGui::MenuItem("1280x720"))
			SetFilmResolution(1280, 720);
		if (ImGui::MenuItem("1920x1080"))
			SetFilmResolution(1920, 1080);

		ImGui::Separator();

		if (ImGui::MenuItem("Use screen resolution")) {
			int currentFrameBufferWidth, currentFrameBufferHeight;
			glfwGetFramebufferSize(window, &currentFrameBufferWidth, &currentFrameBufferHeight);

			SetFilmResolution(currentFrameBufferWidth, currentFrameBufferHeight);
		}

		ImGui::Separator();

		int currentFrameBufferWidth, currentFrameBufferHeight;
		glfwGetFramebufferSize(window, &currentFrameBufferWidth, &currentFrameBufferHeight);
		const float ratio = currentFrameBufferWidth / (float)currentFrameBufferHeight;

		if (ratio >= 1.f)
			ImGui::InputInt("height", &newFilmSize[1]);
		else
			ImGui::InputInt("width", &newFilmSize[0]);

		ImGui::EndMenu();
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Save outputs"))
		session->GetFilm().SaveOutputs();
	if (ImGui::MenuItem("Save film"))
		session->GetFilm().SaveFilm("film.flm");
}

//------------------------------------------------------------------------------
// MenuWindow
//------------------------------------------------------------------------------

void LuxCoreApp::MenuWindow() {
	const string currentEngineType = config->GetProperty("renderengine.type").Get<string>();
	if (ImGui::MenuItem("Sampler editor", NULL, false, !boost::starts_with(currentEngineType, "BIAS")))
		samplerWindow.opened = !samplerWindow.opened;
	if (ImGui::MenuItem("Statistics"))
		statsWindow.opened = !statsWindow.opened;
	if (ImGui::MenuItem("Log console"))
		logWindow.opened = !logWindow.opened;
}

//------------------------------------------------------------------------------
// MenuScreen
//------------------------------------------------------------------------------

void LuxCoreApp::MenuScreen() {
	if (ImGui::BeginMenu("Set refresh interval")) {
		if (ImGui::MenuItem("5ms"))
			config->Parse(Properties().Set(Property("screen.refresh.interval")(5)));
		if (ImGui::MenuItem("10ms"))
			config->Parse(Properties().Set(Property("screen.refresh.interval")(10)));
		if (ImGui::MenuItem("20ms"))
			config->Parse(Properties().Set(Property("screen.refresh.interval")(20)));
		if (ImGui::MenuItem("50ms"))
			config->Parse(Properties().Set(Property("screen.refresh.interval")(50)));
		if (ImGui::MenuItem("100ms"))
			config->Parse(Properties().Set(Property("screen.refresh.interval")(100)));
		if (ImGui::MenuItem("500ms"))
			config->Parse(Properties().Set(Property("screen.refresh.interval")(500)));
		if (ImGui::MenuItem("1000ms"))
			config->Parse(Properties().Set(Property("screen.refresh.interval")(1000)));
		if (ImGui::MenuItem("2000ms"))
			config->Parse(Properties().Set(Property("screen.refresh.interval")(2000)));

		ImGui::EndMenu();
	}
	if (ImGui::MenuItem("Decrease refresh interval","n"))
		DecScreenRefreshInterval();
	if (ImGui::MenuItem("Increase refresh interval","m"))
		IncScreenRefreshInterval();
}

//------------------------------------------------------------------------------
// MainMenuBar
//------------------------------------------------------------------------------

void LuxCoreApp::MainMenuBar() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Rendering")) {
			MenuRendering();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Engine")) {
			MenuEngine();
			ImGui::EndMenu();
		}

		const string currentEngineType = config->GetProperty("renderengine.type").Get<string>();
		if (ImGui::BeginMenu("Sampler", !boost::starts_with(currentEngineType, "BIAS"))) {
			MenuSampler();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Film")) {
			MenuFilm();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Screen")) {
			MenuScreen();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window")) {
			MenuWindow();
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}