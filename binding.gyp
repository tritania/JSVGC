{
  "targets": [
    {
      "target_name": "jsvgc",
      "sources": [ "node_jsvgc.cpp" ],
      "include_dirs" : ["<!(node -e \"require('nan')\")", "lib"]
    }
  ]
}
