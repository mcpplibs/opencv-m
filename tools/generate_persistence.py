#!/usr/bin/env python3
"""Adapt pinned SDK persistence helpers without changing their data formats."""
import argparse
from pathlib import Path
import re
from _files import write_if_changed

ROOT = Path(__file__).resolve().parents[1]


def split_top(text, separator):
    parts, start, depth, quoted, escaped = [], 0, [], '', False
    for i, char in enumerate(text):
        if quoted:
            if escaped:
                escaped = False
            elif char == '\\':
                escaped = True
            elif char == quoted:
                quoted = ''
            continue
        if char in '\"\'':
            quoted = char
        elif char in '(<[':
            depth.append(char)
        elif char in ')>]':
            if depth:
                depth.pop()
        elif char == separator and not depth:
            parts.append(text[start:i]); start = i + 1
    return [*parts, text[start:]]


def without_defaults(text):
    return ', '.join(split_top(part, '=')[0].strip() for part in split_top(text, ','))


def braced_end(text, start):
    depth = 0
    for i in range(start, len(text)):
        if text[i] == '{': depth += 1
        elif text[i] == '}':
            depth -= 1
            if not depth: return i + 1
    raise AssertionError('Unclosed upstream definition')


def adapt_head(head):
    head = re.sub(r'\bstatic\s+inline\s*', '', head).strip()
    if head.startswith('template<'):
        depth = 0
        opening = head.index('<')
        for closing in range(opening, len(head)):
            if head[closing] == '<': depth += 1
            elif head[closing] == '>':
                depth -= 1
                if not depth: break
        assert depth == 0
        args = head[opening + 1:closing]
        head = head[closing + 1:].strip()
        if not args:
            prefix = 'template<> '
        else:
            if 'std::enable_if' in args:
                assert '_Tp' in args
                # The valid SDK parameter type is void*. Keep the optional
                # template argument without exporting its dependent enable_if
                # type, which crashes Clang's module deserializer.
                args = 'typename _Tp, void* _Enable = nullptr'
                constraint = '(std::is_enum_v<_Tp> && opencv_m_pick<_Tp>)'
            else:
                assert '_Tp' in args, args
                constraint = 'opencv_m_pick<_Tp>'
            prefix = f'template<{args}> requires {constraint} '
    else:
        prefix = 'template<class _D = void> requires opencv_m_pick<_D> '
    head = re.sub(r'^inline\s+', '', head)
    signature = ' '.join(head.split())
    assert '(' in signature and ')' in signature, (head, signature)
    left, right = signature.index('('), signature.rindex(')')
    definition = signature[:left + 1] + without_defaults(signature[left + 1:right]) + signature[right:]
    definition_prefix = prefix.replace('_D = void', '_D').replace(' = nullptr', '')
    return prefix + 'inline ' + signature, definition_prefix + 'inline ' + definition


def adapt_body(body):
    body = body.replace('cv::internal::VecWriterProxy', '::cv::opencv_m_storage_detail::VecWriterProxy')
    body = body.replace('cv::internal::VecReaderProxy', '::cv::opencv_m_storage_detail::VecReaderProxy')
    # Preserve custom serializers through ADL only when no built-in adapter
    # handles the type. Built-ins must not rediscover SDK TU-local overloads.
    body = re.sub(r'\bwrite\(\s*fs,\s*(String\(\)|fs\.elname),\s*value\s*\)',
                  r'::cv::opencv_m_storage_detail::write_value(fs, \1, value)', body)
    body = re.sub(r'\bread\(\s*(\*it|n),\s*value,\s*_Tp\(\)\s*\)',
                  r'::cv::opencv_m_storage_detail::read_value(\1, value, _Tp())', body)
    body = re.sub(r'(?<![\w:.])write\s*\(', '::cv::opencv_m_repl::write(', body)
    body = re.sub(r'(?<![\w:.])read\s*\(', '::cv::opencv_m_repl::read(', body)
    # These are the SDK's iterator extraction chains, not shift arithmetic.
    def chain(match):
        expression = 'it'
        for value in match[1].split('>>'):
            expression = f'::cv::opencv_m_repl::operator>>({expression}, {value.strip()})'
        return expression + ';'
    body = re.sub(r'\bit\s*>>\s*([^;]+);', chain, body)
    body = re.sub(r'return \(fs << String\((str|value)\)\);',
                  r'return ::opencv_m_persistence_up::insert_string(fs, String(\1));', body)
    return body


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    source = (ROOT / 'upstream/opencv/modules/core/include/opencv2/core/persistence.hpp').read_text()
    marker = re.compile(r'(?m)^(?:template<[^\n]+>\s*)?static inline\s+|^template<> inline\s+')
    functions = []
    end = 0
    for match in marker.finditer(source):
        if match.start() < end: continue
        opening = source.index('{', match.end()); end = braced_end(source, opening)
        head, body = source[match.start():opening], source[opening:end]
        assert re.search(r'\b(?:read|write)\s*\(|operator\s*(?:<<|>>|-|<)\s*\(', head), head
        declaration, definition = adapt_head(head)
        functions.append((declaration, definition, adapt_body(body)))
    assert len(functions) == 65, len(functions)
    proxy_start = source.index('    template<typename _Tp, int numflag> class VecWriterProxy')
    proxy_end = source.index('} // internal', proxy_start)
    proxies = source[proxy_start:proxy_end].strip()
    proxies = re.sub(r'(?<![\w:.])write\s*\(', '::cv::opencv_m_repl::write(', proxies)
    proxies = re.sub(r'(?<![\w:.])read\s*\(', '::cv::opencv_m_storage_detail::read_value(', proxies)
    copyright = source[:source.index('#ifndef OPENCV_CORE_PERSISTENCE_HPP')]
    extras = '''template<class _D = void> requires opencv_m_pick<_D>
inline FileStorage& operator<<(FileStorage& fs, const String& value);
template<class _D = void> requires opencv_m_pick<_D>
inline void write(FileStorage& fs, const String& name, const std::vector<KeyPoint>& value);
template<class _D = void> requires opencv_m_pick<_D>
inline void write(FileStorage& fs, const String& name, const std::vector<DMatch>& value);
template<class _D = void> requires opencv_m_pick<_D>
inline void read(const FileNode& node, std::vector<KeyPoint>& value);
template<class _D = void> requires opencv_m_pick<_D>
inline void read(const FileNode& node, std::vector<DMatch>& value);
template<class _D = void> requires opencv_m_pick<_D>
inline bool operator==(const FileNodeIterator& a, const FileNodeIterator& b);
template<class _D = void> requires opencv_m_pick<_D>
inline bool operator!=(const FileNodeIterator& a, const FileNodeIterator& b);
'''
    dispatch = '''namespace cv::opencv_m_storage_detail {
template<class T> void write_value(FileStorage& fs, const String& name, const T& value) {
    if constexpr (requires { ::cv::opencv_m_repl::write(fs, name, value); })
        ::cv::opencv_m_repl::write(fs, name, value);
    else write(fs, name, value); // upstream custom-type ADL extension
}
template<class T> void read_value(const FileNode& node, T& value, const T& fallback) {
    if constexpr (requires { ::cv::opencv_m_repl::read(node, value, fallback); })
        ::cv::opencv_m_repl::read(node, value, fallback);
    else read(node, value, fallback); // upstream custom-type ADL extension
}
'''
    result = '// Generated by tools/generate_persistence.py; SDK bodies retained with module-safe lookup.\n' + copyright
    result += '''namespace opencv_m_persistence_up {
inline constexpr auto insert_string = static_cast<cv::FileStorage&(*)(cv::FileStorage&, const cv::String&)>(&cv::operator<<);
inline constexpr auto equal = static_cast<bool(*)(const cv::FileNodeIterator&, const cv::FileNodeIterator&)>(&cv::operator==);
inline constexpr auto unequal = static_cast<bool(*)(const cv::FileNodeIterator&, const cv::FileNodeIterator&)>(&cv::operator!=);
}
export namespace cv { inline namespace opencv_m_repl {
'''
    result += extras + '\n'.join(item[0] + ';' for item in functions) + '\n} }\n'
    result += dispatch + proxies + '\n} // cv::opencv_m_storage_detail\n'
    result += 'export namespace cv { inline namespace opencv_m_repl {\n'
    result += '''template<class _D> requires opencv_m_pick<_D>
inline FileStorage& operator<<(FileStorage& fs, const String& value) { return ::opencv_m_persistence_up::insert_string(fs, value); }
template<class _D> requires opencv_m_pick<_D>
inline void write(FileStorage& fs, const String& name, const std::vector<KeyPoint>& value) { ::opencv_m_fns_up::wr_vkp(fs, name, value); }
template<class _D> requires opencv_m_pick<_D>
inline void write(FileStorage& fs, const String& name, const std::vector<DMatch>& value) { ::opencv_m_fns_up::wr_vdm(fs, name, value); }
template<class _D> requires opencv_m_pick<_D>
inline void read(const FileNode& node, std::vector<KeyPoint>& value) { ::opencv_m_fns_up::rd_vkp(node, value); }
template<class _D> requires opencv_m_pick<_D>
inline void read(const FileNode& node, std::vector<DMatch>& value) { ::opencv_m_fns_up::rd_vdm(node, value); }
template<class _D> requires opencv_m_pick<_D>
inline bool operator==(const FileNodeIterator& a, const FileNodeIterator& b) { return ::opencv_m_persistence_up::equal(a, b); }
template<class _D> requires opencv_m_pick<_D>
inline bool operator!=(const FileNodeIterator& a, const FileNodeIterator& b) { return ::opencv_m_persistence_up::unequal(a, b); }
'''
    result += '\n\n'.join(definition + '\n' + body for _, definition, body in functions)
    result += '\n} } // cv::opencv_m_repl\n'
    output = ROOT / 'src/persistence_ops.inc'
    if args.check:
        assert output.read_text() == result, 'Run python3 tools/generate_persistence.py'
    else:
        write_if_changed(output, result)


if __name__ == '__main__':
    main()
