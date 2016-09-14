��    /      �  C                   1     ?     F     R  &   c     �     �  !   �      �  W   �  �   2  v   �  w   j     �     �     �  f        v     �     �     �     �     �     �     �  S   �  T   N     �     �     �     �  %   �     �     
	     	     *	  U  3	  6  �
  .   �  �  �     �  E   �  W     
   ]  y   h  ;  �  y        �     �     �     �  ;   �     
       '         H  �   ]  �   �  �   �  �   7     �     �     �  �   �     �     �     �  #   �     �     �     �     
  P     P   `     �     �     �  	   �  +   �       
     	   *     4  �  D  :    &   P  s  w     �  D   �  }   @     �  �   �        ,                                 !          $       '                     &               
   )                             +              -       .   (               "   #          *         	          /                 %           A list of audio devices Audio Devices Author Buffer Size Configure Device Configure audio devices used with Jack Defer Dither Do you want to save your changes? EMAIL OF TRANSLATORSYour emails Enable hardware metering for devices that support it.
Otherwise, use software metering. Enable hardware monitoring of capture ports.
This is a method for obtaining "zero latency" monitoring of audio input.
It requires support in hardware and from the underlying ALSA device driver Extra input latency (frames).
This value might be used by audio software to compensate lantency e.g.
during recording. Extra output latency (frames).
This value might be used by audio software to compensate lantency e.g.
during recording. Force 16-bit Hardware Meter Hardware Monitor Ignore xruns reported by the ALSA driver.
This makes JACK less likely to disconnect unresponsive ports Input Channels Input Latency JACK Audio Devices Config Jack failed to start MIDI Driver Monitor NAME OF TRANSLATORSYour names None Number of capture channels.
If set to 0, the maximum supported by hardware is used. Number of playback channels.
If set to 0, the maximum supported by hardware is used. Output Channels Output Latency Periods/Buffer Prefer Provide monitor ports for the output. Rectangular Sample Rate Set dithering mode. Softmode Specify the number of frames between JACK process() calls.
This value must be a power of 2, and the default is 1024.
If you need low latency, set this as low as you can go without seeing xruns.
A larger period size yields higher latency, but makes xruns less likely.
The JACK capture latency in seconds is buffer size divided by sample rate. Specify the number of periods of playback latency.
The default is 2, the minimum allowable.
For most devices, there is no need for any other value.
With boards providing unreliable interrupts, a larger value may yield fewer xruns.
This can also help if the system is not tuned for reliable realtime scheduling. Specify the sample rate.
The default is 48000. Specify which ALSA MIDI system to provide access to.
Using raw will provide a set of JACK MIDI ports that correspond to each raw ALSA device on the machine.
Using seq will provide a set of JACK MIDI ports that correspond to each ALSA "sequencer" client (which includes each hardware MIDI port on the machine).
raw provides slightly better performance but does not permit JACK MIDI communication with software written to use the ALSA "sequencer" API. Switch Master The configuration of the currently selected device has been modified. This audio device is not compatible with the current settings. Please try other values. Triangular Try to configure card for 16-bit samples first, only trying 32-bits if unsuccessful.
Default is to prefer 32-bit samples. Project-Id-Version: 0.1
Report-Msgid-Bugs-To: 
POT-Creation-Date: 2016-09-14 18:23+0200
PO-Revision-Date: 2016-08-22 14:46+0200
Last-Translator: Julian Wolff <wolff@julianwolff.de>
Language-Team: de <LL@li.org>
Language: de
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: 16bit
 Liste von Audiogeräten. Die Reihenfolge der Geräte in dieser Liste gibt an, welche Geräte als Master bevorzugt werden. Audiogeräte Autor Puffergröße Gerät konfigurieren Konfiguration der Audiogeräte, die von Jack benutzt werden Zurückstellen Dither Möchten Sie die Änderungen speichern? wolff@julianwolff.de Aktiviere Hardware-Pegelmessung für Geräte, die dies unterstützen.
Ansonsten wird eine softwareseitige Pegelmessung verwendet. Aktiviere Hardware-Monitoring der Eingangskanäle.
Hierdurch wird ein Abhören der Eingangssignale "ohne Latenz" ermöglicht.
Diese Einstellung erfordert Unterstützung der Hardware und des Gerätetreibers. Eingangslatenz (frames).
Dieser Wert kann von Audio-Software verwendet werden, um Latenzen z.B. während einer Aufnahme zu kompensieren. Ausgangslatenz (frames).
Dieser Wert kann von Audio-Software verwendet werden, um Latenzen z.B. während einer Aufnahme zu kompensieren. 16-bit erzwingen Hardware-Pegelmessung Hardware-Monitor Ignoriere xruns, die vom ALSA-Treiber berichtet werden.
Hierdurch sinkt die Wahrscheinlichkeit, dass JACK nicht reagierende Ports abschaltet. Eingangskanäle Eingangslatenz Konfiguration der Audiogeräte JACK konnte nicht gestartet werden. MIDI-Treiber Monitor Julian Wolff Kein Anzahl der Eingangskanäle.
Falls 0, werden alle verfügbaren Kanäle verwendet. Anzahl der Ausgangskanäle.
Falls 0, werden alle verfügbaren Kanäle verwendet. Ausgangskanäle Ausgangslatenz Perioden/Puffer Vorziehen Stelle zusätzliche Monitor-Kanäle bereit. Rechteck Abtastrate Dithering Ignoriere XRuns Anzahl frames zwischen zwei Aufrufen von JACK process().
Dieser Wert muss eine Zweierpotenz sein. Voreinstellung ist 1024
Für geringe Latenz sollte dieser Wert so niedrig gesetzt werden, wie es ohne Auftreten von xruns möglich ist.
Eine größere Puffergröße führt zu einer höheren Latenz, senkt aber gleichzeitig die Wahrscheinlichkeit für xruns.
Die Aufnahmelatenz für JACK in Sekunden berechnet sich aus der Puffergröße dividiert durch die Abtastrate. Anzahl an Perioden der Wiedergabelatenz
Voreinstellung und der niedrigst mögliche Wert ist 2.
Für die meisten Geräte ist dieser Wert ausreichend.
Für Geräte mit unzuverlässigen Interrupts können höhere Werte benötigt werden.
Dies kann helfen, falls das System nicht für Echtzeitbedingungen optimiert ist. Abtastrate. Voreinstellung ist 48000Hz ALSA MIDI-Treiber.
"ALSA Raw-MIDI" stellt für alle ALSA-Geräte im System JACK MIDI ports zur Verfügung.
"ALSA Sequencer" stellt für alle ALSA-Sequencer Clients (was alle ALSA-Hardwaregeräte einschließt) JACK MIDI ports zur Verfügung.
ALSA Raw-MIDI ist geringfügig performanter, lässt jedoch keine Verbindung mit Software zu, die die ALSA Sequencer API verwendet. Master wechseln Die Konfiguration des aktuell ausgewählten Geräts wurde geändert. Dieses Gerät ist mit den gewählten Einstellungen nicht kompatibel. Bitte versuchen Sie es mit anderen Einstellungen erneut. Dreieck Versuche zunächst 16-bit samples zu verwenden. Verwende 32-bit nur falls dies fehlschlägt.
Falls deaktiviert werden 32-bit bevorzugt. 