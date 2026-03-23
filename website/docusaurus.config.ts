import {themes as prismThemes} from 'prism-react-renderer';
import type {Config} from '@docusaurus/types';
import type * as Preset from '@docusaurus/preset-classic';

const config: Config = {
  title: 'C++ 设计模式',
  tagline: '基于"封装变化"分类法的 GoF 23 种设计模式教学',
  favicon: 'img/favicon.ico',

  url: 'https://tskysheep.github.io',
  baseUrl: '/LearnDesignPatterns/',

  organizationName: 'Tskysheep',
  projectName: 'LearnDesignPatterns',

  onBrokenLinks: 'throw',
  onBrokenMarkdownLinks: 'warn',

  i18n: {
    defaultLocale: 'zh-Hans',
    locales: ['zh-Hans'],
    localeConfigs: {
      'zh-Hans': {
        label: '简体中文',
        direction: 'ltr',
        htmlLang: 'zh-Hans',
      },
    },
  },

  markdown: {
    mermaid: true,
  },

  themes: [
    '@docusaurus/theme-mermaid',
    [
      '@easyops-cn/docusaurus-search-local',
      {
        hashed: true,
        language: ['en', 'zh'],
        highlightSearchTermsOnTargetPage: true,
        explicitSearchResultPath: true,
      },
    ],
  ],

  presets: [
    [
      'classic',
      {
        docs: {
          sidebarPath: './sidebars.ts',
          breadcrumbs: true,
          routeBasePath: 'docs',
        },
        blog: false,
        theme: {
          customCss: './src/css/custom.css',
        },
      } satisfies Preset.Options,
    ],
  ],

  themeConfig: {
    navbar: {
      title: 'C++ 设计模式',
      items: [
        {
          type: 'docSidebar',
          sidebarId: 'docs',
          position: 'left',
          label: '教程',
        },
      ],
    },
    docs: {
      sidebar: {
        hideable: true,
      },
    },
    footer: {
      style: 'dark',
      links: [
        {
          title: '文档',
          items: [
            {label: '设计模式概论', to: '/docs/intro'},
            {label: '学习路线', to: '/docs/learning-path'},
          ],
        },
        {
          title: '分类',
          items: [
            {label: '组件协作', to: '/docs/category/组件协作'},
            {label: '对象创建', to: '/docs/category/对象创建'},
            {label: '接口隔离', to: '/docs/category/接口隔离'},
          ],
        },
      ],
      copyright: `Copyright © ${new Date().getFullYear()} C++ 设计模式教学项目`,
    },
    prism: {
      theme: prismThemes.github,
      darkTheme: prismThemes.dracula,
      additionalLanguages: ['bash', 'json', 'cpp', 'cmake'],
    },
  } satisfies Preset.ThemeConfig,
};

export default config;
