---
layout: page
title: PiCaSO
parent: Projects
---

# PiCaSO
The dominance of machine learning and the ending of Moore’s law have renewed interests in Processor in Memory (PIM) architectures. 
This interest has produced several recent proposals to modify an FPGA’s BRAM architecture to form a next-generation PIM reconfigurable fabric. 
PIM architectures can also be realized within today’s FPGAs as overlays without the need to modify the underlying FPGA architecture. 
To date, there has been no study to understand the comparative advantages of the two approaches. 
In this work, we present a study that explores the comparative advantages between two proposed custom architectures and a PIM overlay running on a commodity FPGA. 
We created PiCaSO, a Processor in/near Memory Scalable and Fast Overlay architecture as a representative PIM overlay. 
The results of this study show that the PiCaSO overlay achieves up to 80% of the peak throughput of the custom designs with 2.56× 
shorter latency and 25% – 43% better BRAM memory utilization efficiency. 
We then show how several key features of the PiCaSO overlay can be integrated into the custom PIM designs 
to further improve their throughput by 18%, latency by 19.5%, and memory efficiency by 6.2%.

## Related Publications
- MD Arafat Kabir, Ehsan Kabir, Joshua Hollis, Eli Levy-Mackay, Atiyehsadat Panahi, Jason Bakos, Miaoqing Huang, and David Andrews, "FPGA Processor In Memory Architectures (PIMs): Overlay or Overhaul?" The 33rd International Conference on Field-Programmable Logic and Applications (FPL 2023), pp. 1-7, September 2023. [arXiv](https://doi.org/10.48550/arXiv.2308.03914)
- MD Arafat Kabir, Joshua Hollis, Atiyehsadat Panahi, Jason Bakos, Miaoqing Huang, and David Andrews, "Making BRAMs Compute: Creating Scalable Computational Memory Fabric Overlays," The 31st IEEE International Symposium On Field-Programmable Custom Computing Machines (FCCM 2023), pp. 1-1, May 2023. [IEEE](https://doi.org/10.1109/FCCM57271.2023.00052)


<!-- NSF Acknowledgement -->
<span style="display: inline-flex; align-items: center;">
  <a href="https://www.nsf.gov/awardsearch/showAward?AWD_ID=1955820&HistoricalAwards" target="_blank" style="text-decoration: none;">
    <img src="/assets/images/NSF_logo.svg" alt="NSF Logo" style="width: 60px; height: 60px; margin-right: 10px;">
  </a>
  <span style="display: inline-block; vertical-align: middle;">
    Development of PiCaSO is supported by National Science Foundation <br>
    under <a href="https://www.nsf.gov/awardsearch/showAward?AWD_ID=1955820&HistoricalAwards" target="_blank">Grant No. 1955820</a>.
  </span>
</span>

<!-- Github URL -->
<span style="display: inline-flex; align-items: center;">
  <a href="https://github.com/Arafat-Kabir/PiCaSO" target="_blank" style="text-decoration: none;">
    <img src="/assets/images/github_logo.png" alt="Github Logo" style="width: 60px; height: 60px; margin-right: 10px;">
  </a>
  <span style="display: inline-block; vertical-align: middle;">
    Check out PiCaSO on 
    <a href="https://github.com/Arafat-Kabir/PiCaSO" target="_blank">Github</a>.
  </span>
</span>
