--混沌の黒魔術師
function c40737112.initial_effect(c)
	--summon success
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(40737112,0))
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1,40737112)
	e1:SetCost(c40737112.thcost)
	e1:SetTarget(c40737112.thtg)
	e1:SetOperation(c40737112.thop)
	c:RegisterEffect(e1)
	--remove
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(40737112,1))
	e2:SetCategory(CATEGORY_REMOVE)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_BATTLED)
	e2:SetCondition(c40737112.rmcon)
	e2:SetTarget(c40737112.rmtg)
	e2:SetOperation(c40737112.rmop)
	c:RegisterEffect(e2)
	--redirect
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_LEAVE_FIELD_REDIRECT)
	e3:SetCondition(c40737112.recon)
	e3:SetValue(LOCATION_REMOVED)
	c:RegisterEffect(e3)
	if not c40737112.global_check then
		c40737112.global_check=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_SUMMON_SUCCESS)
		ge1:SetOperation(c40737112.checkop)
		Duel.RegisterEffect(ge1,0)
		local ge2=ge1:Clone()
		ge2:SetCode(EVENT_SPSUMMON_SUCCESS)
		Duel.RegisterEffect(ge2,0)
	end
end
function c40737112.checkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	while tc do
		tc:RegisterFlagEffect(40737112,RESET_EVENT+0x1ec0000+RESET_PHASE+PHASE_END,0,1)
		tc=eg:GetNext()
	end
end
function c40737112.thcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetFlagEffect(40737112)~=0 end
	e:GetHandler():ResetFlagEffect(40737112)
end
function c40737112.thfilter(c)
	return c:IsType(TYPE_SPELL) and c:IsAbleToHand()
end
function c40737112.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c40737112.thfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c40737112.thfilter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c40737112.thfilter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c40737112.thop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end
function c40737112.rmcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local bc=c:GetBattleTarget()
	e:SetLabelObject(bc)
	return bc and bc:IsStatus(STATUS_BATTLE_DESTROYED) and c:IsStatus(STATUS_OPPO_BATTLE)
end
function c40737112.rmtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,e:GetLabelObject(),1,0,0)
end
function c40737112.rmop(e,tp,eg,ep,ev,re,r,rp)
	local bc=e:GetLabelObject()
	if bc:IsRelateToBattle() then
		Duel.Remove(bc,POS_FACEUP,REASON_EFFECT)
	end
end
function c40737112.recon(e)
	return e:GetHandler():IsFaceup()
end
