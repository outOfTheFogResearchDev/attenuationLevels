import React, { Component } from 'react';
import styled from 'styled-components';

const LevelForm = styled.form`
  padding: 5px 5px;
  margin: 10px 0;
  box-sizing: border-box;
`; // margin-left 25px base, +186 per channel past 1 (done in react style bellow)

const LevelText = styled.label`
  display: inline-block;
  font-size: 125%;
`;

const LevelValue = styled.input`
  display: inline-block;
  width: 40px;
  margin-left: 15px;
  transform: scale(1.5);
`;

export default class extends Component {
  constructor(props) {
    super(props);

    this.handleChange = this.handleChange.bind(this);
  }

  async handleChange({ target: { value: code } }) {
    const { handleCodeChange, handleTempCodeChange, level } = this.props; // eslint-disable-line react/prop-types
    if (code >= 90 && code <= 200) await handleCodeChange(level, code);
    else handleTempCodeChange(level, code);
  }

  render() {
    const { level, code, channel } = this.props; // eslint-disable-line react/prop-types
    const marginLeft = `${25 + 186 * (channel - 1)}px`; // 25px base, +186 for every channel past 1
    return (
      <LevelForm style={{ marginLeft }}>
        <LevelText>Level {(+level < 10 ? '0' : '') + level}:</LevelText>
        <LevelValue type="number" min="90" max="200" value={code} onChange={this.handleChange} />
      </LevelForm>
    );
  }
}
