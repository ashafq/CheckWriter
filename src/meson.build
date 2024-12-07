checkwriter_sources = [
  'main.c',
  'checkwriter-application.c',
  'checkwriter-window.c',
  'check-properties.c',
  'num-to-words.c'
]

if host_machine.system() == 'linux'
    message('Building for Linux')
    m = cc.find_library('m', required: true)  # Find the math library (libm) on Linux
else
    message('Not Linux - skipping libm')
    m = null  # No dependency for non-Linux systems
endif

checkwriter_deps = [
  dependency('gtk4'),
  dependency('libadwaita-1', version: '>= 1.4'),
  m,
]

checkwriter_sources += gnome.compile_resources('checkwriter-resources',
  'checkwriter.gresource.xml',
  c_name: 'checkwriter'
)

executable('checkwriter', checkwriter_sources,
  dependencies: checkwriter_deps,
       install: true,
)
