#include "MidiPlayer.h"
#include <fm/config.hpp>
#include <fmapp/os.hpp>
#include <fm/config.hpp>
#include <thread>
#include <iostream>
#include <algorithm>

#define UPDATE_THREAD_SLEEPTIME 70

namespace fmapp {

    void MidiPlayer::listDevices(std::ostream& os)
    {
        auto outputs = _midiPlayerImpl.getOutputs();
        for (const auto &output : outputs) {
            os << output.id << ": " << output.name << std::endl;
        }
    }

    void MidiPlayer::write(sheet::DocumentPtr doc)
    {
        if (state > Stopped) {
            return;
        }
        _logger->babble(WMLogLambda(log << "start playback"));
        _logger->babble(WMLogLambda(log << "midi: " << _midifile->byteSize() << " bytes"));
        execLoop(doc);
    }

    void MidiPlayer::execLoop(sheet::DocumentPtr)
    {
        fm::Ticks resume = 0;
        fm::Ticks begin = 0;
        fm::Ticks end = _midifile->duration();
        if (_programOptions->isBeginSet()) {
            begin = std::max(fm::Ticks(0), std::min(_programOptions->getBegin() * fm::PPQ, end));
        }
        if (_programOptions->isEndSet()) {
            end = std::max(fm::Ticks(0), std::min(_programOptions->getEnd() * fm::PPQ, end));
        }
        if (_programOptions->getResumeAtPosition() > 0) {
            resume = _programOptions->getResumeAtPosition() * fm::PPQ;
            _programOptions->setResumeAtPosition(0);
        }        
        _logger->babble(WMLogLambda(log << "begin at tick: " << begin));
        _midiPlayerImpl.updateOutputMapping(fm::getConfigServer().getDevices());
        _midiPlayerImpl.midi(_midifile);
        _midiPlayerImpl.play(resume > 0 ? resume : begin);

        state = Playing;
        bool loop   = _programOptions->isLoopSet();
        fmapp::os::setSigtermHandler([&]{
            state = Stopped;
            _midiPlayerImpl.panic();
        });

#ifdef SIGINT_WORKAROUND
        // std::unique_ptr<fmapp::os::InterProcessMessageQueue> ipcMessageQueue = nullptr;
        // bool isSigintWorkaround = false; // TODO: #126
        // if (isSigintWorkaround) {
        //     ipcMessageQueue = std::make_unique<fmapp::os::InterProcessMessageQueue>();
        // }
#endif
        visitVisitors(BeginLoop, 0);
        while (state > Stopped) {
            auto elapsed = _midiPlayerImpl.elapsed();
            std::this_thread::sleep_for( std::chrono::milliseconds(UPDATE_THREAD_SLEEPTIME) );
#ifdef SIGINT_WORKAROUND
            // if (ipcMessageQueue && ipcMessageQueue->sigintReceived()) {
            //     playing = false;
            //     player.panic();
            // }
#endif
            if (elapsed >= end) {
                if (!loop) {
                    break;
                }
                _midiPlayerImpl.play(begin);
            }
            visitVisitors(Loop, elapsed);
        }
        visitVisitors(EndLoop, 0);
        _midiPlayerImpl.stop();
        _midiPlayerImpl.panic();
        _midiPlayerImpl.Backend:: tearDown();

    }

    void MidiPlayer::visitVisitors(VisitorMessage msg, fm::Ticks elapsed)
    {
        for(auto visitor : _loopVisitors.container) {
            switch (msg)
            {
                case BeginLoop: 
                    visitor->loopBegin();
                    break;
                case EndLoop:
                    visitor->loopEnd();
                    break;
                case Loop:
                    visitor->visit(elapsed);
            }
        }
    }

    fm::Ticks MidiPlayer::stop()
    {
        if (state == Stopped) {
            return 0;
        }
        auto position = _midiPlayerImpl.elapsed();
        _midiPlayerImpl.stop();
        state = Stopped;
        return position;
    }
}