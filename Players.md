If you want any player be supported, please fire an [issue](http://code.google.com/p/osd-lyrics/issues/list) instead of leaving message here :)

| **Player**                  | **URI** | **Get status** | **Play** | **Pause** | **Stop** | **Previous** | **Next** | **Seek** |
|:----------------------------|:--------|:---------------|:---------|:----------|:---------|:-------------|:---------|:---------|
| Amarok 1.4<sup><code>[1]</code></sup>| No      | No             | No       | No        | Yes      | No           | No       | No       |
| Amarok 2.x                  | Yes     | Yes            | Yes      | Yes       | Yes      | Yes          | Yes      | Yes      |
| Audacious                   | Yes     | Yes            | Yes      | Yes       | Yes      | Yes          | Yes      | Yes      |
| Banshee                     | Yes     | Yes            | Yes      | Yes       | Yes      | Yes          | Yes      | Yes      |
| Clementine                  | Yes     | Yes            | Yes      | Yes       | Yes      | Yes          | Yes      | Yes      |
| Deadbeef<sup><code>[8]</code></sup>  | Yes     | Yes            | Yes      | Yes       | Yes      | Yes          | Yes      | Yes      |
| Deciber<sup><code>[6]</code></sup>   | Yes     | Yes            | Yes      | Yes       | Yes      | Yes          | Yes      | Yes      |
| Exaile 0.2                  | No      | Yes            | No       | No        | No       | No           | No       | No       |
| Exaile 0.3                  | Yes     | Yes            | Yes      | Yes       | Yes      | Yes          | Yes      | No       |
| Guayadeque 2.5              | Yes     | Yes            | Yes      | Yes       | Yes      | Yes          | Yes      | Yes      |
| Gmusicbrowser               | Yes     | Yes            | Yes      | Yes       | Yes      | Yes          | Yes      | Yes      |
| Juk                         | Yes     | Yes            | Yes      | Yes       | Yes      | Yes          | Yes      | No       |
| Listen                      | Yes     | Yes            | Yes      | Yes       | Yes      | Yes          | Yes      | No       |
| MPD<sup><code>[2]</code></sup>       | No      | Yes            | Yes      | Yes       | Yes      | Yes          | Yes      | Yes      |
| MOC<sup><code>[3]</code></sup>       | Yes     | Yes            | Yes      | Yes       | No       | Yes          | Yes      | No       |
| Muine                       | Yes     | Yes            | Yes      | Yes       | No       | Yes          | Yes      | Yes      |
| Qmmp                        | Yes     | Yes            | Yes      | Yes       | Yes      | Yes          | Yes      | Yes      |
| Quod Libet                  | No      | Yes            | Yes      | Yes       | No       | Yes          | Yes      | No       |
| Rhythmbox                   | Yes     | Yes            | Yes      | Yes       | Yes      | Yes          | Yes      | Yes      |
| Songbird<sup><code>[4]</code></sup>  | Yes     | Yes            | Yes      | Yes       | Yes      | Yes          | Yes      | Yes      |
| vlc<sup><code>[7]</code></sup>       | Yes     | Yes            | Yes      | Yes       | Yes      | Yes          | Yes      | Yes      |
| XMMS2<sup><code>[5]</code></sup>     | Yes     | Yes            | Yes      | Yes       | Yes      | Yes          | Yes      | Yes      |

`[1]`. Amarok 1.4 support is disabled by default, to enable it, you must pass --enable-amarok1 in the configure phase. See [build instructions](Build#Available_Options.md) for details.

`[2]`. MPD support requires libmpd installed. OSD Lyrics respects MPD\_HOST and MPD\_PORT environment variables.

`[3]`. MOC support eats a lot of CPU resource due to cost the of pipe and fork. Its stop feature is disables because of OSD Lyrics won't respond when MOC stops.

`[4]`. Songbird support requires its [MPRIS extension](http://addons.songbirdnest.com/addon/1626)

`[5]`. XMMS2 support requires xmms-client installed.

`[6]`. Require Deciber 1.05 or later

`[7]`. Run with parameter: "--control dbus"

`[8]`. You need to install [deadbeef-mpris-plugin](https://code.google.com/p/deadbeef-mpris-plugin/) to support Deadbeef