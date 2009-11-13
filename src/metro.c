<!doctype html public "-//W3C//DTD HTML 3.2//EN">
<html>
<head>
<title>JACK/example-clients/metro.c</title>
<base href="http://jackit.sourceforge.net/cgi-bin/lxr/http/">
</head>

<body bgcolor=white>
<div align=center>
   ~ <b>[</b>&nbsp;<b><i>source navigation</i></b>&nbsp;<b>]</b> ~ <b>[</b>&nbsp;<a href="diff/example-clients/metro.c">diff markup</a>&nbsp;<b>]</b> ~ <b>[</b>&nbsp;<a href="ident">identifier search</a>&nbsp;<b>]</b> ~ <b>[</b>&nbsp;<a href="search">freetext search</a>&nbsp;<b>]</b> ~ <b>[</b>&nbsp;<a href="find">file search</a>&nbsp;<b>]</b> ~
</div>

<h1 align=center>
  <a href="http://www.linux.org/"> 
    Linux</a>
  <a href="http:blurb.html">
    Cross Reference</a><br>
  <a href="source/">JACK</a>/<a href="source/example-clients/">example-clients</a>/<a href="source/example-clients/metro.c">metro.c</a>
</h1>

<div align=center>
  
</div>
<hr>
<pre><h4 align="center"><i>** Warning: Cannot open xref database.</i></h4>
  <a name=L1 href="source/example-clients/metro.c#L1">1</a> <b><i>/*</i></b>
  <a name=L2 href="source/example-clients/metro.c#L2">2</a> <b><i>    Copyright (C) 2002 Anthony Van Groningen</i></b>
  <a name=L3 href="source/example-clients/metro.c#L3">3</a> <b><i>    </i></b>
  <a name=L4 href="source/example-clients/metro.c#L4">4</a> <b><i>    This program is free software; you can redistribute it and/or modify</i></b>
  <a name=L5 href="source/example-clients/metro.c#L5">5</a> <b><i>    it under the terms of the GNU General Public License as published by</i></b>
  <a name=L6 href="source/example-clients/metro.c#L6">6</a> <b><i>    the Free Software Foundation; either version 2 of the License, or</i></b>
  <a name=L7 href="source/example-clients/metro.c#L7">7</a> <b><i>    (at your option) any later version.</i></b>
  <a name=L8 href="source/example-clients/metro.c#L8">8</a> <b><i></i></b>
  <a name=L9 href="source/example-clients/metro.c#L9">9</a> <b><i>    This program is distributed in the hope that it will be useful,</i></b>
 <a name=L10 href="source/example-clients/metro.c#L10">10</a> <b><i>    but WITHOUT ANY WARRANTY; without even the implied warranty of</i></b>
 <a name=L11 href="source/example-clients/metro.c#L11">11</a> <b><i>    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the</i></b>
 <a name=L12 href="source/example-clients/metro.c#L12">12</a> <b><i>    GNU General Public License for more details.</i></b>
 <a name=L13 href="source/example-clients/metro.c#L13">13</a> <b><i></i></b>
 <a name=L14 href="source/example-clients/metro.c#L14">14</a> <b><i>    You should have received a copy of the GNU General Public License</i></b>
 <a name=L15 href="source/example-clients/metro.c#L15">15</a> <b><i>    along with this program; if not, write to the Free Software</i></b>
 <a name=L16 href="source/example-clients/metro.c#L16">16</a> <b><i>    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.</i></b>
 <a name=L17 href="source/example-clients/metro.c#L17">17</a> <b><i>*/</i></b>
 <a name=L18 href="source/example-clients/metro.c#L18">18</a> 
 <a name=L19 href="source/example-clients/metro.c#L19">19</a> #include &lt;stdlib.h&gt;
 <a name=L20 href="source/example-clients/metro.c#L20">20</a> #include &lt;stdio.h&gt;
 <a name=L21 href="source/example-clients/metro.c#L21">21</a> #include &lt;errno.h&gt;
 <a name=L22 href="source/example-clients/metro.c#L22">22</a> #include &lt;unistd.h&gt;
 <a name=L23 href="source/example-clients/metro.c#L23">23</a> #include &lt;math.h&gt;
 <a name=L24 href="source/example-clients/metro.c#L24">24</a> #include &lt;jack/jack.h&gt;
 <a name=L25 href="source/example-clients/metro.c#L25">25</a> #include &lt;jack/transport.h&gt;
 <a name=L26 href="source/example-clients/metro.c#L26">26</a> #include &lt;getopt.h&gt;
 <a name=L27 href="source/example-clients/metro.c#L27">27</a> #include &lt;string.h&gt;
 <a name=L28 href="source/example-clients/metro.c#L28">28</a> 
 <a name=L29 href="source/example-clients/metro.c#L29">29</a> typedef jack_default_audio_sample_t sample_t;
 <a name=L30 href="source/example-clients/metro.c#L30">30</a> 
 <a name=L31 href="source/example-clients/metro.c#L31">31</a> const double PI = 3.14;
 <a name=L32 href="source/example-clients/metro.c#L32">32</a> 
 <a name=L33 href="source/example-clients/metro.c#L33">33</a> jack_client_t *client;
 <a name=L34 href="source/example-clients/metro.c#L34">34</a> jack_port_t *output_port;
 <a name=L35 href="source/example-clients/metro.c#L35">35</a> unsigned long sr;
 <a name=L36 href="source/example-clients/metro.c#L36">36</a> int freq = 880;
 <a name=L37 href="source/example-clients/metro.c#L37">37</a> int bpm;
 <a name=L38 href="source/example-clients/metro.c#L38">38</a> jack_nframes_t tone_length, wave_length;
 <a name=L39 href="source/example-clients/metro.c#L39">39</a> sample_t *wave;
 <a name=L40 href="source/example-clients/metro.c#L40">40</a> long offset = 0;
 <a name=L41 href="source/example-clients/metro.c#L41">41</a> int transport_aware = 0;
 <a name=L42 href="source/example-clients/metro.c#L42">42</a> jack_transport_state_t transport_state;
 <a name=L43 href="source/example-clients/metro.c#L43">43</a> 
 <a name=L44 href="source/example-clients/metro.c#L44">44</a> void
 <a name=L45 href="source/example-clients/metro.c#L45">45</a> usage ()
 <a name=L46 href="source/example-clients/metro.c#L46">46</a> 
 <a name=L47 href="source/example-clients/metro.c#L47">47</a> {
 <a name=L48 href="source/example-clients/metro.c#L48">48</a>         fprintf (stderr, <i>"\n"</i>
 <a name=L49 href="source/example-clients/metro.c#L49">49</a> <i>"usage: jack_metro \n"</i>
 <a name=L50 href="source/example-clients/metro.c#L50">50</a> <i>"              [ --frequency OR -f frequency (in Hz) ]\n"</i>
 <a name=L51 href="source/example-clients/metro.c#L51">51</a> <i>"              [ --amplitude OR -A maximum amplitude (between 0 and 1) ]\n"</i>
 <a name=L52 href="source/example-clients/metro.c#L52">52</a> <i>"              [ --duration OR -D duration (in ms) ]\n"</i>
 <a name=L53 href="source/example-clients/metro.c#L53">53</a> <i>"              [ --attack OR -a attack (in percent of duration) ]\n"</i>
 <a name=L54 href="source/example-clients/metro.c#L54">54</a> <i>"              [ --decay OR -d decay (in percent of duration) ]\n"</i>
 <a name=L55 href="source/example-clients/metro.c#L55">55</a> <i>"              [ --name OR -n jack name for metronome client ]\n"</i>
 <a name=L56 href="source/example-clients/metro.c#L56">56</a> <i>"              [ --transport OR -t transport aware ]\n"</i>
 <a name=L57 href="source/example-clients/metro.c#L57">57</a> <i>"              --bpm OR -b beats per minute\n"</i>
 <a name=L58 href="source/example-clients/metro.c#L58">58</a> );
 <a name=L59 href="source/example-clients/metro.c#L59">59</a> }
 <a name=L60 href="source/example-clients/metro.c#L60">60</a> 
 <a name=L61 href="source/example-clients/metro.c#L61">61</a> void
 <a name=L62 href="source/example-clients/metro.c#L62">62</a> process_silence (jack_nframes_t nframes) 
 <a name=L63 href="source/example-clients/metro.c#L63">63</a> {
 <a name=L64 href="source/example-clients/metro.c#L64">64</a>         sample_t *buffer = (sample_t *) jack_port_get_buffer (output_port, nframes);
 <a name=L65 href="source/example-clients/metro.c#L65">65</a>         memset (buffer, 0, sizeof (jack_default_audio_sample_t) * nframes);
 <a name=L66 href="source/example-clients/metro.c#L66">66</a> }
 <a name=L67 href="source/example-clients/metro.c#L67">67</a> 
 <a name=L68 href="source/example-clients/metro.c#L68">68</a> void
 <a name=L69 href="source/example-clients/metro.c#L69">69</a> process_audio (jack_nframes_t nframes) 
 <a name=L70 href="source/example-clients/metro.c#L70">70</a> {
 <a name=L71 href="source/example-clients/metro.c#L71">71</a> 
 <a name=L72 href="source/example-clients/metro.c#L72">72</a>         sample_t *buffer = (sample_t *) jack_port_get_buffer (output_port, nframes);
 <a name=L73 href="source/example-clients/metro.c#L73">73</a>         jack_nframes_t frames_left = nframes;
 <a name=L74 href="source/example-clients/metro.c#L74">74</a>                 
 <a name=L75 href="source/example-clients/metro.c#L75">75</a>         while (wave_length - offset &lt; frames_left) {
 <a name=L76 href="source/example-clients/metro.c#L76">76</a>                 memcpy (buffer + (nframes - frames_left), wave + offset, sizeof (sample_t) * (wave_length - offset));
 <a name=L77 href="source/example-clients/metro.c#L77">77</a>                 frames_left -= wave_length - offset;
 <a name=L78 href="source/example-clients/metro.c#L78">78</a>                 offset = 0;
 <a name=L79 href="source/example-clients/metro.c#L79">79</a>         }
 <a name=L80 href="source/example-clients/metro.c#L80">80</a>         if (frames_left &gt; 0) {
 <a name=L81 href="source/example-clients/metro.c#L81">81</a>                 memcpy (buffer + (nframes - frames_left), wave + offset, sizeof (sample_t) * frames_left);
 <a name=L82 href="source/example-clients/metro.c#L82">82</a>                 offset += frames_left;
 <a name=L83 href="source/example-clients/metro.c#L83">83</a>         }
 <a name=L84 href="source/example-clients/metro.c#L84">84</a> }
 <a name=L85 href="source/example-clients/metro.c#L85">85</a> 
 <a name=L86 href="source/example-clients/metro.c#L86">86</a> int
 <a name=L87 href="source/example-clients/metro.c#L87">87</a> process (jack_nframes_t nframes, void *arg)
 <a name=L88 href="source/example-clients/metro.c#L88">88</a> {
 <a name=L89 href="source/example-clients/metro.c#L89">89</a>         if (transport_aware) {
 <a name=L90 href="source/example-clients/metro.c#L90">90</a>                 jack_position_t pos;
 <a name=L91 href="source/example-clients/metro.c#L91">91</a> 
 <a name=L92 href="source/example-clients/metro.c#L92">92</a>                 if (jack_transport_query (client, &amp;pos)
 <a name=L93 href="source/example-clients/metro.c#L93">93</a>                     != JackTransportRolling) {
 <a name=L94 href="source/example-clients/metro.c#L94">94</a> 
 <a name=L95 href="source/example-clients/metro.c#L95">95</a>                         process_silence (nframes);
 <a name=L96 href="source/example-clients/metro.c#L96">96</a>                         return 0;
 <a name=L97 href="source/example-clients/metro.c#L97">97</a>                 }
 <a name=L98 href="source/example-clients/metro.c#L98">98</a>                 offset = pos.frame % wave_length;
 <a name=L99 href="source/example-clients/metro.c#L99">99</a>         }
<a name=L100 href="source/example-clients/metro.c#L100">100</a>         process_audio (nframes);
<a name=L101 href="source/example-clients/metro.c#L101">101</a>         return 0;
<a name=L102 href="source/example-clients/metro.c#L102">102</a> }
<a name=L103 href="source/example-clients/metro.c#L103">103</a> 
<a name=L104 href="source/example-clients/metro.c#L104">104</a> int
<a name=L105 href="source/example-clients/metro.c#L105">105</a> sample_rate_change () {
<a name=L106 href="source/example-clients/metro.c#L106">106</a>         printf(<i>"Sample rate has changed! Exiting...\n"</i>);
<a name=L107 href="source/example-clients/metro.c#L107">107</a>         exit(-1);
<a name=L108 href="source/example-clients/metro.c#L108">108</a> }
<a name=L109 href="source/example-clients/metro.c#L109">109</a> 
<a name=L110 href="source/example-clients/metro.c#L110">110</a> int
<a name=L111 href="source/example-clients/metro.c#L111">111</a> main (int argc, char *argv[])
<a name=L112 href="source/example-clients/metro.c#L112">112</a> {
<a name=L113 href="source/example-clients/metro.c#L113">113</a>         
<a name=L114 href="source/example-clients/metro.c#L114">114</a>         sample_t scale;
<a name=L115 href="source/example-clients/metro.c#L115">115</a>         int i, attack_length, decay_length;
<a name=L116 href="source/example-clients/metro.c#L116">116</a>         double *amp;
<a name=L117 href="source/example-clients/metro.c#L117">117</a>         double max_amp = 0.5;
<a name=L118 href="source/example-clients/metro.c#L118">118</a>         int option_index;
<a name=L119 href="source/example-clients/metro.c#L119">119</a>         int opt;
<a name=L120 href="source/example-clients/metro.c#L120">120</a>         int got_bpm = 0;
<a name=L121 href="source/example-clients/metro.c#L121">121</a>         int attack_percent = 1, decay_percent = 10, dur_arg = 100;
<a name=L122 href="source/example-clients/metro.c#L122">122</a>         char *client_name = 0;
<a name=L123 href="source/example-clients/metro.c#L123">123</a>         char *bpm_string = <i>"bpm"</i>;
<a name=L124 href="source/example-clients/metro.c#L124">124</a>         int verbose = 0;
<a name=L125 href="source/example-clients/metro.c#L125">125</a> 
<a name=L126 href="source/example-clients/metro.c#L126">126</a>         const char *options = <i>"f:A:D:a:d:b:n:thv"</i>;
<a name=L127 href="source/example-clients/metro.c#L127">127</a>         struct option long_options[] =
<a name=L128 href="source/example-clients/metro.c#L128">128</a>         {
<a name=L129 href="source/example-clients/metro.c#L129">129</a>                 {<i>"frequency"</i>, 1, 0, <i>'f'</i>},
<a name=L130 href="source/example-clients/metro.c#L130">130</a>                 {<i>"amplitude"</i>, 1, 0, <i>'A'</i>},
<a name=L131 href="source/example-clients/metro.c#L131">131</a>                 {<i>"duration"</i>, 1, 0, <i>'D'</i>},
<a name=L132 href="source/example-clients/metro.c#L132">132</a>                 {<i>"attack"</i>, 1, 0, <i>'a'</i>},
<a name=L133 href="source/example-clients/metro.c#L133">133</a>                 {<i>"decay"</i>, 1, 0, <i>'d'</i>},
<a name=L134 href="source/example-clients/metro.c#L134">134</a>                 {<i>"bpm"</i>, 1, 0, <i>'b'</i>},
<a name=L135 href="source/example-clients/metro.c#L135">135</a>                 {<i>"name"</i>, 1, 0, <i>'n'</i>},
<a name=L136 href="source/example-clients/metro.c#L136">136</a>                 {<i>"transport"</i>, 0, 0, <i>'t'</i>},
<a name=L137 href="source/example-clients/metro.c#L137">137</a>                 {<i>"help"</i>, 0, 0, <i>'h'</i>},
<a name=L138 href="source/example-clients/metro.c#L138">138</a>                 {<i>"verbose"</i>, 0, 0, <i>'v'</i>},
<a name=L139 href="source/example-clients/metro.c#L139">139</a>                 {0, 0, 0, 0}
<a name=L140 href="source/example-clients/metro.c#L140">140</a>         };
<a name=L141 href="source/example-clients/metro.c#L141">141</a>         
<a name=L142 href="source/example-clients/metro.c#L142">142</a>         while ((opt = getopt_long (argc, argv, options, long_options, &amp;option_index)) != EOF) {
<a name=L143 href="source/example-clients/metro.c#L143">143</a>                 switch (opt) {
<a name=L144 href="source/example-clients/metro.c#L144">144</a>                 case <i>'f'</i>:
<a name=L145 href="source/example-clients/metro.c#L145">145</a>                         if ((freq = atoi (optarg)) &lt;= 0) {
<a name=L146 href="source/example-clients/metro.c#L146">146</a>                                 fprintf (stderr, <i>"invalid frequency\n"</i>);
<a name=L147 href="source/example-clients/metro.c#L147">147</a>                                 return -1;
<a name=L148 href="source/example-clients/metro.c#L148">148</a>                         }
<a name=L149 href="source/example-clients/metro.c#L149">149</a>                         break;
<a name=L150 href="source/example-clients/metro.c#L150">150</a>                 case <i>'A'</i>:
<a name=L151 href="source/example-clients/metro.c#L151">151</a>                         if (((max_amp = atof (optarg)) &lt;= 0)|| (max_amp &gt; 1)) {
<a name=L152 href="source/example-clients/metro.c#L152">152</a>                                 fprintf (stderr, <i>"invalid amplitude\n"</i>);
<a name=L153 href="source/example-clients/metro.c#L153">153</a>                                 return -1;
<a name=L154 href="source/example-clients/metro.c#L154">154</a>                         }
<a name=L155 href="source/example-clients/metro.c#L155">155</a>                         break;
<a name=L156 href="source/example-clients/metro.c#L156">156</a>                 case <i>'D'</i>:
<a name=L157 href="source/example-clients/metro.c#L157">157</a>                         dur_arg = atoi (optarg);
<a name=L158 href="source/example-clients/metro.c#L158">158</a>                         fprintf (stderr, <i>"durarg = %u\n"</i>, dur_arg);
<a name=L159 href="source/example-clients/metro.c#L159">159</a>                         break;
<a name=L160 href="source/example-clients/metro.c#L160">160</a>                 case <i>'a'</i>:
<a name=L161 href="source/example-clients/metro.c#L161">161</a>                         if (((attack_percent = atoi (optarg)) &lt; 0) || (attack_percent &gt; 100)) {
<a name=L162 href="source/example-clients/metro.c#L162">162</a>                                 fprintf (stderr, <i>"invalid attack percent\n"</i>);
<a name=L163 href="source/example-clients/metro.c#L163">163</a>                                 return -1;
<a name=L164 href="source/example-clients/metro.c#L164">164</a>                         }
<a name=L165 href="source/example-clients/metro.c#L165">165</a>                         break;
<a name=L166 href="source/example-clients/metro.c#L166">166</a>                 case <i>'d'</i>:
<a name=L167 href="source/example-clients/metro.c#L167">167</a>                         if (((decay_percent = atoi (optarg)) &lt; 0) || (decay_percent &gt; 100)) {
<a name=L168 href="source/example-clients/metro.c#L168">168</a>                                 fprintf (stderr, <i>"invalid decay percent\n"</i>);
<a name=L169 href="source/example-clients/metro.c#L169">169</a>                                 return -1;
<a name=L170 href="source/example-clients/metro.c#L170">170</a>                         }
<a name=L171 href="source/example-clients/metro.c#L171">171</a>                         break;
<a name=L172 href="source/example-clients/metro.c#L172">172</a>                 case <i>'b'</i>:
<a name=L173 href="source/example-clients/metro.c#L173">173</a>                         got_bpm = 1;
<a name=L174 href="source/example-clients/metro.c#L174">174</a>                         if ((bpm = atoi (optarg)) &lt; 0) {
<a name=L175 href="source/example-clients/metro.c#L175">175</a>                                 fprintf (stderr, <i>"invalid bpm\n"</i>);
<a name=L176 href="source/example-clients/metro.c#L176">176</a>                                 return -1;
<a name=L177 href="source/example-clients/metro.c#L177">177</a>                         }
<a name=L178 href="source/example-clients/metro.c#L178">178</a>                         bpm_string = (char *) malloc ((strlen (optarg) + 4) * sizeof (char));
<a name=L179 href="source/example-clients/metro.c#L179">179</a>                         strcpy (bpm_string, optarg);
<a name=L180 href="source/example-clients/metro.c#L180">180</a>                         strcat (bpm_string, <i>"_bpm"</i>);
<a name=L181 href="source/example-clients/metro.c#L181">181</a>                         break;
<a name=L182 href="source/example-clients/metro.c#L182">182</a>                 case <i>'n'</i>:
<a name=L183 href="source/example-clients/metro.c#L183">183</a>                         client_name = (char *) malloc (strlen (optarg) * sizeof (char));
<a name=L184 href="source/example-clients/metro.c#L184">184</a>                         strcpy (client_name, optarg);
<a name=L185 href="source/example-clients/metro.c#L185">185</a>                         break;
<a name=L186 href="source/example-clients/metro.c#L186">186</a>                 case <i>'v'</i>:
<a name=L187 href="source/example-clients/metro.c#L187">187</a>                         verbose = 1;
<a name=L188 href="source/example-clients/metro.c#L188">188</a>                         break;
<a name=L189 href="source/example-clients/metro.c#L189">189</a>                 case <i>'t'</i>:
<a name=L190 href="source/example-clients/metro.c#L190">190</a>                         transport_aware = 1;
<a name=L191 href="source/example-clients/metro.c#L191">191</a>                         break;
<a name=L192 href="source/example-clients/metro.c#L192">192</a>                 default:
<a name=L193 href="source/example-clients/metro.c#L193">193</a>                         fprintf (stderr, <i>"unknown option %c\n"</i>, opt); 
<a name=L194 href="source/example-clients/metro.c#L194">194</a>                 case <i>'h'</i>:
<a name=L195 href="source/example-clients/metro.c#L195">195</a>                         usage ();
<a name=L196 href="source/example-clients/metro.c#L196">196</a>                         return -1;
<a name=L197 href="source/example-clients/metro.c#L197">197</a>                 }
<a name=L198 href="source/example-clients/metro.c#L198">198</a>         }
<a name=L199 href="source/example-clients/metro.c#L199">199</a>         if (!got_bpm) {
<a name=L200 href="source/example-clients/metro.c#L200">200</a>                 fprintf (stderr, <i>"bpm not specified\n"</i>);
<a name=L201 href="source/example-clients/metro.c#L201">201</a>                 usage ();
<a name=L202 href="source/example-clients/metro.c#L202">202</a>                 return -1;
<a name=L203 href="source/example-clients/metro.c#L203">203</a>         }
<a name=L204 href="source/example-clients/metro.c#L204">204</a> 
<a name=L205 href="source/example-clients/metro.c#L205">205</a>         <b><i>/* Initial Jack setup, get sample rate */</i></b>
<a name=L206 href="source/example-clients/metro.c#L206">206</a>         if (!client_name) {
<a name=L207 href="source/example-clients/metro.c#L207">207</a>                 client_name = (char *) malloc (9 * sizeof (char));
<a name=L208 href="source/example-clients/metro.c#L208">208</a>                 strcpy (client_name, <i>"metro"</i>);
<a name=L209 href="source/example-clients/metro.c#L209">209</a>         }
<a name=L210 href="source/example-clients/metro.c#L210">210</a>         if ((client = jack_client_new (client_name)) == 0) {
<a name=L211 href="source/example-clients/metro.c#L211">211</a>                 fprintf (stderr, <i>"jack server not running?\n"</i>);
<a name=L212 href="source/example-clients/metro.c#L212">212</a>                 return 1;
<a name=L213 href="source/example-clients/metro.c#L213">213</a>         }
<a name=L214 href="source/example-clients/metro.c#L214">214</a>         jack_set_process_callback (client, process, 0);
<a name=L215 href="source/example-clients/metro.c#L215">215</a>         output_port = jack_port_register (client, bpm_string, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
<a name=L216 href="source/example-clients/metro.c#L216">216</a> 
<a name=L217 href="source/example-clients/metro.c#L217">217</a>         sr = jack_get_sample_rate (client);
<a name=L218 href="source/example-clients/metro.c#L218">218</a> 
<a name=L219 href="source/example-clients/metro.c#L219">219</a>         <b><i>/* setup wave table parameters */</i></b>
<a name=L220 href="source/example-clients/metro.c#L220">220</a>         wave_length = 60 * sr / bpm;
<a name=L221 href="source/example-clients/metro.c#L221">221</a>         tone_length = sr * dur_arg / 1000;
<a name=L222 href="source/example-clients/metro.c#L222">222</a>         attack_length = tone_length * attack_percent / 100;
<a name=L223 href="source/example-clients/metro.c#L223">223</a>         decay_length = tone_length * decay_percent / 100;
<a name=L224 href="source/example-clients/metro.c#L224">224</a>         scale = 2 * PI * freq / sr;
<a name=L225 href="source/example-clients/metro.c#L225">225</a> 
<a name=L226 href="source/example-clients/metro.c#L226">226</a>         if (tone_length &gt;= wave_length) {
<a name=L227 href="source/example-clients/metro.c#L227">227</a>                 fprintf (stderr, <i>"invalid duration (tone length = %"</i> PRIu32
<a name=L228 href="source/example-clients/metro.c#L228">228</a>                          <i>", wave length = %"</i> PRIu32 <i>"\n"</i>, tone_length,
<a name=L229 href="source/example-clients/metro.c#L229">229</a>                          wave_length);
<a name=L230 href="source/example-clients/metro.c#L230">230</a>                 return -1;
<a name=L231 href="source/example-clients/metro.c#L231">231</a>         }
<a name=L232 href="source/example-clients/metro.c#L232">232</a>         if (attack_length + decay_length &gt; (int)tone_length) {
<a name=L233 href="source/example-clients/metro.c#L233">233</a>                 fprintf (stderr, <i>"invalid attack/decay\n"</i>);
<a name=L234 href="source/example-clients/metro.c#L234">234</a>                 return -1;
<a name=L235 href="source/example-clients/metro.c#L235">235</a>         }
<a name=L236 href="source/example-clients/metro.c#L236">236</a> 
<a name=L237 href="source/example-clients/metro.c#L237">237</a>         <b><i>/* Build the wave table */</i></b>
<a name=L238 href="source/example-clients/metro.c#L238">238</a>         wave = (sample_t *) malloc (wave_length * sizeof(sample_t));
<a name=L239 href="source/example-clients/metro.c#L239">239</a>         amp = (double *) malloc (tone_length * sizeof(double));
<a name=L240 href="source/example-clients/metro.c#L240">240</a> 
<a name=L241 href="source/example-clients/metro.c#L241">241</a>         for (i = 0; i &lt; attack_length; i++) {
<a name=L242 href="source/example-clients/metro.c#L242">242</a>                 amp[i] = max_amp * i / ((double) attack_length);
<a name=L243 href="source/example-clients/metro.c#L243">243</a>         }
<a name=L244 href="source/example-clients/metro.c#L244">244</a>         for (i = attack_length; i &lt; (int)tone_length - decay_length; i++) {
<a name=L245 href="source/example-clients/metro.c#L245">245</a>                 amp[i] = max_amp;
<a name=L246 href="source/example-clients/metro.c#L246">246</a>         }
<a name=L247 href="source/example-clients/metro.c#L247">247</a>         for (i = (int)tone_length - decay_length; i &lt; (int)tone_length; i++) {
<a name=L248 href="source/example-clients/metro.c#L248">248</a>                 amp[i] = - max_amp * (i - (double) tone_length) / ((double) decay_length);
<a name=L249 href="source/example-clients/metro.c#L249">249</a>         }
<a name=L250 href="source/example-clients/metro.c#L250">250</a>         for (i = 0; i &lt; (int)tone_length; i++) {
<a name=L251 href="source/example-clients/metro.c#L251">251</a>                 wave[i] = amp[i] * sin (scale * i);
<a name=L252 href="source/example-clients/metro.c#L252">252</a>         }
<a name=L253 href="source/example-clients/metro.c#L253">253</a>         for (i = tone_length; i &lt; (int)wave_length; i++) {
<a name=L254 href="source/example-clients/metro.c#L254">254</a>                 wave[i] = 0;
<a name=L255 href="source/example-clients/metro.c#L255">255</a>         }
<a name=L256 href="source/example-clients/metro.c#L256">256</a> 
<a name=L257 href="source/example-clients/metro.c#L257">257</a>         if (jack_activate (client)) {
<a name=L258 href="source/example-clients/metro.c#L258">258</a>                 fprintf (stderr, <i>"cannot activate client"</i>);
<a name=L259 href="source/example-clients/metro.c#L259">259</a>                 return 1;
<a name=L260 href="source/example-clients/metro.c#L260">260</a>         }
<a name=L261 href="source/example-clients/metro.c#L261">261</a> 
<a name=L262 href="source/example-clients/metro.c#L262">262</a>         while (1) {
<a name=L263 href="source/example-clients/metro.c#L263">263</a>                 sleep(1);
<a name=L264 href="source/example-clients/metro.c#L264">264</a>         };
<a name=L265 href="source/example-clients/metro.c#L265">265</a>         
<a name=L266 href="source/example-clients/metro.c#L266">266</a> }
<a name=L267 href="source/example-clients/metro.c#L267">267</a> </pre><hr>
<div align=center>
   ~ <b>[</b>&nbsp;<b><i>source navigation</i></b>&nbsp;<b>]</b> ~ <b>[</b>&nbsp;<a href="diff/example-clients/metro.c">diff markup</a>&nbsp;<b>]</b> ~ <b>[</b>&nbsp;<a href="ident">identifier search</a>&nbsp;<b>]</b> ~ <b>[</b>&nbsp;<a href="search">freetext search</a>&nbsp;<b>]</b> ~ <b>[</b>&nbsp;<a href="find">file search</a>&nbsp;<b>]</b> ~
</div>

<hr>
This page was automatically generated by the 
<a href="http:blurb.html">LXR engine</a>.
<br>
Visit the <a href="http://lxr.linux.no/">LXR main site</a> for more
information.
</html>
