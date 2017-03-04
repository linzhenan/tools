grep -oE "/av-arch/jctvc-site/bitstream_exchange/draft_conformance/HEVC_v1/[^\"]*zip" index.html | while read -r line; do wget http://wftp3.itu.int$line; done
