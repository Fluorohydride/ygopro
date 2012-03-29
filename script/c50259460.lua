--破壊神 ヴァサーゴ
function c50259460.initial_effect(c)
	--fusion substitute
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(0x1e)
	e1:SetCode(EFFECT_FUSION_SUBSTITUTE)
	c:RegisterEffect(e1)
end
