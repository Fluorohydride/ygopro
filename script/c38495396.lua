--セイクリッド・トレミスM7
function c38495396.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,nil,6,2,c38495396.ovfilter,aux.Stringid(38495396,1),2,c38495396.xyzop)
	c:EnableReviveLimit()
	--to hand
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(38495396,0))
	e2:SetCategory(CATEGORY_TOHAND)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetCost(c38495396.thcost)
	e2:SetTarget(c38495396.thtg)
	e2:SetOperation(c38495396.thop)
	c:RegisterEffect(e2)
end
function c38495396.ovfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x53) and c:GetCode()~=38495396 and c:IsType(TYPE_XYZ)
end
function c38495396.xyzop(e,tp,chk)
	if chk==0 then return true end
	e:GetHandler():RegisterFlagEffect(38495396,RESET_EVENT+0xfe0000+RESET_PHASE+PHASE_END,0,1)
end
function c38495396.thcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c38495396.thfilter(c)
	return c:IsType(TYPE_MONSTER) and c:IsAbleToHand()
end
function c38495396.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE+LOCATION_GRAVE) and c38495396.thfilter(chkc) end
	if chk==0 then return e:GetHandler():GetFlagEffect(38495396)==0
		and Duel.IsExistingTarget(c38495396.thfilter,tp,LOCATION_MZONE+LOCATION_GRAVE,LOCATION_MZONE+LOCATION_GRAVE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local g=Duel.SelectTarget(tp,c38495396.thfilter,tp,LOCATION_MZONE+LOCATION_GRAVE,LOCATION_MZONE+LOCATION_GRAVE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c38495396.thop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
	end
end
