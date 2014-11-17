--影霊衣の舞姫
function c52738610.initial_effect(c)
	--chain limit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_CHAINING)
	e1:SetCondition(c52738610.chcon)
	e1:SetOperation(c52738610.chop)
	c:RegisterEffect(e1)
	--cannot be target
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_CANNOT_BE_EFFECT_TARGET)
	e2:SetRange(LOCATION_MZONE)
	e2:SetTargetRange(LOCATION_MZONE,0)
	e2:SetTarget(c52738610.efftg)
	e2:SetValue(1)
	c:RegisterEffect(e2)
	--to hand
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(52738610,0))
	e3:SetCategory(CATEGORY_TOHAND)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e3:SetCode(EVENT_RELEASE)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
	e3:SetCountLimit(1,52738610)
	e3:SetCondition(c52738610.thcon)
	e3:SetTarget(c52738610.thtg)
	e3:SetOperation(c52738610.thop)
	c:RegisterEffect(e3)
end
function c52738610.chcon(e,tp,eg,ep,ev,re,r,rp)
	local rc=re:GetHandler()
	return bit.band(rc:GetType(),0x82)==0x82 and rc:IsSetCard(0xb4)
end
function c52738610.chop(e,tp,eg,ep,ev,re,r,rp)
	Duel.SetChainLimit(c52738610.chlimit)
end
function c52738610.chlimit(e,ep,tp)
	return ep==tp
end
function c52738610.efftg(e,c)
	return bit.band(c:GetType(),0x81)==0x81 and c:IsSetCard(0xb4)
end
function c52738610.thcon(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(r,REASON_EFFECT)~=0
end
function c52738610.thfilter(c)
	return c:IsSetCard(0xb4) and c:IsType(TYPE_MONSTER) and not c:IsCode(52738610) and c:IsAbleToHand() and c:IsFaceup()
end
function c52738610.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_REMOVED) and chkc:IsControler(tp) and c52738610.thfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c52738610.thfilter,tp,LOCATION_REMOVED,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	tg=Duel.SelectTarget(tp,c52738610.thfilter,tp,LOCATION_REMOVED,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,tg,1,tp,LOCATION_REMOVED)
end
function c52738610.thop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end
