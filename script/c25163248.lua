--先史遺産マヤン・マシーン
function c25163248.initial_effect(c)
	--double tribute
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DOUBLE_TRIBUTE)
	e1:SetValue(c25163248.condition)
	c:RegisterEffect(e1)
end
function c25163248.condition(e,c)
	return c:IsRace(RACE_MACHINE)
end
