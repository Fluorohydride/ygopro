--破滅の女神ルイン
function c46427957.initial_effect(c)
	c:EnableReviveLimit()
	--chain attack
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(46427957,0))
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_BATTLED)
	e1:SetCondition(c46427957.atcon)
	e1:SetOperation(c46427957.atop)
	c:RegisterEffect(e1)
end
function c46427957.atcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local bc=c:GetBattleTarget()
	return bc and bc:IsStatus(STATUS_BATTLE_DESTROYED) and c:IsChainAttackable() and c:IsStatus(STATUS_OPPO_BATTLE)
end
function c46427957.atop(e,tp,eg,ep,ev,re,r,rp)
	Duel.ChainAttack()
end
