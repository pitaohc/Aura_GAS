---
name: create-sub-issues
description: 自动创建章节的小节issues，包括milestone创建和子issue关联
license: MIT
compatibility: opencode
metadata:
  audience: maintainers
  workflow: github
---

## 功能描述
这个skill用于自动化创建章节的小节issues，包括以下步骤：
1. 检查当前issue是否为章节issue（格式为"xx-0 xxx"）
2. 创建milestone（格式为"第x节 xxxx"）
3. 从issue内容中识别小节列表（支持图片和文字列表）
4. 为每个小节创建子issue，并设置parent和milestone

## 使用场景
当需要为课程章节创建小节issues时使用此skill。例如：
- 章节issue标题："7-0 Gameplay Tags"
- 自动创建milestone："第7节 Gameplay Tags"
- 从issue内容中识别小节列表
- 创建子issues："7-49 Gameplay Tags"、"7-50 Creating Gameplay Tags in the Editor"等

## 执行步骤

### 步骤1：检查issue标题
- 读取当前issue的标题
- 检查是否符合"xx-0 xxx"格式（如"7-0 Gameplay Tags"）
- 如果不符合，自动退出

### 步骤2：创建milestone
- 从issue标题中提取章节号和标题
- 创建milestone，格式为"第x节 xxxx"
- 关联milestone和章节issue

### 步骤3：识别小节列表
从issue内容中识别小节列表，支持两种情况：
1. **图片列表**：识别图片中的小节名称
2. **文字列表**：识别文字格式的小节列表

### 步骤4：创建子issues
- 将小节名称转换为"x-x xxx"格式（如"7-49 Gameplay Tags"）
- 为每个小节创建issue（只需要标题，不需要描述）
- 设置parent为章节issue
- 设置milestone与章节issue相同

## 示例
输入：章节issue "7-0 Gameplay Tags"
输出：
- 创建milestone "第7节 Gameplay Tags"
- 创建子issues：
  - "7-49 Gameplay Tags"
  - "7-50 Creating Gameplay Tags in the Editor"
  - "7-51 Creating Gameplay Tags from Data Tables"
  - ...（根据实际内容）

## 注意事项
- 确保issue标题符合格式要求
- 正确识别图片中的文字内容
- 子issue的编号应与章节号一致
- 所有子issue应关联到相同的milestone