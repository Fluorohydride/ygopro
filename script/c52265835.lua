--A・O・J ルドラ
function c52265835.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetCondition(c52265835.condtion)
	e1:SetValue(700)
	c:RegisterEffect(e1)
end
function c52265835.condtion(e)
	local ph=Duel.GetCurrentPhase()
	if not (ph==PHASE_DAMAGE or ph==PHASE_DAMAGE_CAL) then return false end
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	return (a==e:GetHandler() and d and d:IsFaceup() and d:IsAttribute(ATTRIBUTE_LIGHT))
		or (d==e:GetHandler() and a:IsAttribute(ATTRIBUTE_LIGHT))
end
