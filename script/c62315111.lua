--エーリアン·ハンター
function c62315111.initial_effect(c)
	--chain attack
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(62315111,0))
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_BATTLED)
	e1:SetCondition(c62315111.atcon)
	e1:SetOperation(c62315111.atop)
	c:RegisterEffect(e1)
end
function c62315111.atcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local bc=c:GetBattleTarget()
	return bc and bc:IsStatus(STATUS_BATTLE_DESTROYED) and c:IsChainAttackable()
		and bc:GetCounter(0xe)>0
end
function c62315111.atop(e,tp,eg,ep,ev,re,r,rp)
	Duel.ChainAttack()
end
