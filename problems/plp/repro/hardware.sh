#!/bin/bash
# Registra el hardware y el software de la maquina (para la seccion experimental del paper).
# uso: repro/hardware.sh [archivo]   (por defecto repro/out/hardware.txt)
OUT=${1:-$(dirname "$0")/out/hardware.txt}
mkdir -p "$(dirname "$OUT")"
{
echo "# fecha: $(date -Is)"
echo "# host: $(hostname)"
echo; echo "## CPU (lscpu)"; lscpu 2>/dev/null
echo; echo "## nucleos"
echo "logicos (nproc): $(nproc)"
phys=$(lscpu -p=Core,Socket 2>/dev/null | grep -v '^#' | sort -u | wc -l)
echo "fisicos (core,socket distintos): $phys"
echo; echo "## frecuencias por nucleo (MHz, ahora)"; grep MHz /proc/cpuinfo | awk '{print $4}' | sort -n | uniq -c
for g in /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor /sys/devices/system/cpu/intel_pstate/no_turbo /sys/devices/system/cpu/cpufreq/boost; do
  [ -r "$g" ] && echo "$g: $(cat $g)"
done
echo; echo "## memoria"; head -3 /proc/meminfo; free -h 2>/dev/null
echo; echo "## sistema"; cat /etc/os-release 2>/dev/null | grep -E '^(PRETTY_NAME|VERSION)='; uname -a
echo; echo "## carga actual"; uptime
echo; echo "## compiladores y herramientas"
g++ --version 2>/dev/null | head -1; cmake --version 2>/dev/null | head -1; python3 --version 2>/dev/null
echo; echo "## repositorio"; git -C "$(dirname "$0")" log -1 --format='%H %s' 2>/dev/null
git -C "$(dirname "$0")" status --short 2>/dev/null | head
} > "$OUT" 2>&1
echo "hardware en $OUT"
grep -E '^(Model name|Socket|Core|Thread|CPU max MHz|CPU\(s\)|logicos|fisicos)' "$OUT"
