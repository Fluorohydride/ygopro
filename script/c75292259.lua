--ポセイドン・オオカブト
function c75292259.initial_effect(c)
	--chain attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_DAMAGE_STEP_END)
	e1:SetCondition(c75292259.atcon)
	e1:SetOperation(c75292259.atop)
	c:RegisterEffect(e1)
end
function c75292259.atcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local bc=c:GetBattleTarget()
	return c==Duel.GetAttacker() and bc and  bc:IsRelateToBattle()
		and bc:GetBattlePosition()==POS_FACEUP_ATTACK and c:IsChainAttackable(3)
end
function c75292259.atop(e,tp,eg,ep,ev,re,r,rp)
	Duel.ChainAttack(e:GetHandler():GetBattleTarget())
end
