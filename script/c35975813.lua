--ジェノサイドキングデーモン
function c35975813.initial_effect(c)
	--sumlimit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CANNOT_SUMMON)
	e1:SetCondition(c35975813.excon)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_CANNOT_FLIP_SUMMON)
	c:RegisterEffect(e2)
	--maintain
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e3:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCountLimit(1)
	e3:SetOperation(c35975813.mtop)
	c:RegisterEffect(e3)
	--disable and destroy
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e4:SetCode(EVENT_CHAIN_SOLVING)
	e4:SetRange(LOCATION_MZONE)
	e4:SetOperation(c35975813.disop)
	c:RegisterEffect(e4)
	--disable
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e5:SetCode(EVENT_BATTLED)
	e5:SetRange(LOCATION_MZONE)
	e5:SetOperation(c35975813.disop2)
	c:RegisterEffect(e5)
end
function c35975813.exfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x45)
end
function c35975813.excon(e)
	return not Duel.IsExistingMatchingCard(c35975813.exfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
end
function c35975813.mtop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetTurnPlayer()~=tp then return end
	if Duel.CheckLPCost(tp,800) then
		Duel.PayLPCost(tp,800)
	else
		Duel.Destroy(e:GetHandler(),REASON_RULE)
	end
end
function c35975813.disop(e,tp,eg,ep,ev,re,r,rp)
	if ep==tp then return end
	if not re:IsHasProperty(EFFECT_FLAG_CARD_TARGET) then return false end
	local tg=Duel.GetChainInfo(ev,CHAININFO_TARGET_CARDS)
	if not tg or not tg:IsContains(e:GetHandler()) or not Duel.IsChainDisablable(ev) then return false end
	local rc=re:GetHandler()
	local dc=Duel.TossDice(tp,1)
	if dc~=2 and dc~=5 then return end
	Duel.NegateEffect(ev)
	if rc:IsRelateToEffect(re) then
		Duel.Destroy(rc,REASON_EFFECT)
	end
end
function c35975813.disop2(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=c:GetBattleTarget()
	if tc and tc:IsType(TYPE_EFFECT) and tc:IsStatus(STATUS_BATTLE_DESTROYED) then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_DISABLE)
		e1:SetReset(RESET_EVENT+0x17a0000)
		tc:RegisterEffect(e1)
		local e2=Effect.CreateEffect(c)
		e2:SetType(EFFECT_TYPE_SINGLE)
		e2:SetCode(EFFECT_DISABLE_EFFECT)
		e2:SetReset(RESET_EVENT+0x17a0000)
		tc:RegisterEffect(e2)
	end
end
