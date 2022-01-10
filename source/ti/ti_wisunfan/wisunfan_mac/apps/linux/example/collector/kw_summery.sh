
output=collector.kw

rm -rf ${output}

kwinject -t ../../components/common/objs/klockwork/kw.output --output ${output}
kwinject --update -t ../../components/nv/objs/klockwork/kw.output --output ${output}
kwinject --update -t ../../components/api/objs/klockwork/kw.output --output ${output}
kwinject --update -t ./objs/klockwork/kw.output --output ${output}



