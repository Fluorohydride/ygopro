--誤封の契約書
function c37209439.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c37209439.target)
	c:RegisterEffect(e1)
	--disable
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(37209439,0))
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetCountLimit(1)
	e2:SetCondition(c37209439.negcon)
	e2:SetCost(c37209439.negcost)
	e2:SetOperation(c37209439.negop)
	c:RegisterEffect(e2)
	--damage
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(37209439,1))
	e3:SetCategory(CATEGORY_DAMAGE)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e3:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCountLimit(1)
	e3:SetCondition(c37209439.damcon)
	e3:SetCost(c37209439.damcost)
	e3:SetTarget(c37209439.damtg)
	e3:SetOperation(c37209439.damop)
	c:RegisterEffect(e3)
end
function c37209439.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local b1=c37209439.negcon(e,tp,eg,ep,ev,re,r,rp)
	local b2=c37209439.damcon(e,tp,eg,ep,ev,re,r,rp) and Duel.GetCurrentPhase()==PHASE_STANDBY
	if (b1 or b2) and Duel.SelectYesNo(tp,94) then
		local c=e:GetHandler()
		local op=0
		if b1 and b2 then
			op=Duel.SelectOption(tp,aux.Stringid(37209439,0),aux.Stringid(37209439,1))
		elseif b1 then
			op=Duel.SelectOption(tp,aux.Stringid(37209439,0))
		else op=Duel.SelectOption(tp,aux.Stringid(37209439,1))+1 end
		if op==0 then
			c:RegisterFlagEffect(37209439,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
			e:SetCategory(0)
			e:SetOperation(c37209439.negop)
		else
			c:RegisterFlagEffect(37209440,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
			c37209439.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
			e:SetCategory(CATEGORY_DAMAGE)
			e:SetOperation(c37209439.damop)
		end
	else
		e:SetCategory(0)
		e:SetOperation(nil)
	end
end
function c37209439.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0xaf)
end
function c37209439.negcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c37209439.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c37209439.negcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetFlagEffect(37209439)==0 end
	e:GetHandler():RegisterFlagEffect(37209439,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
end
function c37209439.negop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	local fid=c:GetFieldID()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_DISABLE)
	e1:SetRange(LOCATION_SZONE)
	e1:SetTargetRange(LOCATION_SZONE,LOCATION_SZONE)
	e1:SetTarget(c37209439.distg)
	e1:SetLabel(fid)
	e1:SetReset(RESET_PHASE+RESET_END)
	Duel.RegisterEffect(e1,tp)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_CHAIN_SOLVING)
	e2:SetRange(LOCATION_SZONE)
	e2:SetOperation(c37209439.disop)
	e2:SetLabel(fid)
	e2:SetReset(RESET_PHASE+RESET_END)
	Duel.RegisterEffect(e2,tp)
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_DISABLE_TRAPMONSTER)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e3:SetTarget(c37209439.distg)
	e3:SetLabel(fid)
	e3:SetReset(RESET_PHASE+RESET_END)
	Duel.RegisterEffect(e3,tp)
end
function c37209439.distg(e,c)
	return c:GetFieldID()~=e:GetLabel() and c:IsType(TYPE_TRAP) and c:IsStatus(STATUS_ACTIVATED)
end
function c37209439.disop(e,tp,eg,ep,ev,re,r,rp)
	local tl=Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_LOCATION)
	if tl==LOCATION_SZONE and re:IsActiveType(TYPE_TRAP) and re:GetHandler():GetFieldID()~=e:GetLabel() then
		Duel.NegateEffect(ev)
	end
end
function c37209439.damcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp
end
function c37209439.damcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetFlagEffect(37209440)==0 end
	e:GetHandler():RegisterFlagEffect(37209440,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
end
function c37209439.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(1000)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,0,0,tp,1000)
end
function c37209439.damop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
