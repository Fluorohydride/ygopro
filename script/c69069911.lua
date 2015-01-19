--カチコチドラゴン
function c69069911.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,nil,4,2)
	c:EnableReviveLimit()
	--chain attack
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(69069911,0))
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_BATTLE_DESTROYING)
	e1:SetCountLimit(1)
	e1:SetCondition(c69069911.atcon)
	e1:SetCost(c69069911.atcost)
	e1:SetOperation(c69069911.atop)
	c:RegisterEffect(e1)
end
function c69069911.atcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local bc=c:GetBattleTarget()
	return bc:IsLocation(LOCATION_GRAVE) and bc:IsType(TYPE_MONSTER) and c:IsChainAttackable() and c:IsStatus(STATUS_OPPO_BATTLE)
end
function c69069911.atcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c69069911.atop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToBattle() then return end
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e1:SetCode(EFFECT_EXTRA_ATTACK)
	e1:SetValue(1)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_BATTLE)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_ATTACK_ANNOUNCE)
	e2:SetLabelObject(e1)
	e2:SetOperation(aux.atrst)
	e2:SetReset(RESET_PHASE+PHASE_BATTLE)
	Duel.RegisterEffect(e2,tp)
end
