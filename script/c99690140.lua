--墓守の従者
function c99690140.initial_effect(c)
	--battle damage to effect damage
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_BATTLE_DAMAGE_TO_EFFECT)
	c:RegisterEffect(e1)
end
