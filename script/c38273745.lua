--ヴェルズ・ウロボロス
function c38273745.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,nil,4,3)
	c:EnableReviveLimit()
	--tohand
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(38273745,1))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1,EFFECT_COUNT_CODE_SINGLE)
	e1:SetCondition(c38273745.con1)
	e1:SetCost(c38273745.cost)
	e1:SetTarget(c38273745.tg1)
	e1:SetOperation(c38273745.op1)
	c:RegisterEffect(e1)
	--handes
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(38273745,2))
	e2:SetCategory(CATEGORY_TOGRAVE)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1,EFFECT_COUNT_CODE_SINGLE)
	e2:SetCondition(c38273745.con2)
	e2:SetCost(c38273745.cost)
	e2:SetTarget(c38273745.tg2)
	e2:SetOperation(c38273745.op2)
	c:RegisterEffect(e2)
	--remove
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(38273745,3))
	e3:SetCategory(CATEGORY_REMOVE)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCountLimit(1,EFFECT_COUNT_CODE_SINGLE)
	e3:SetCondition(c38273745.con3)
	e3:SetCost(c38273745.cost)
	e3:SetTarget(c38273745.tg3)
	e3:SetOperation(c38273745.op3)
	c:RegisterEffect(e3)
end
function c38273745.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	Duel.Hint(HINT_OPSELECTED,1-tp,e:GetDescription())
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c38273745.con1(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetFlagEffect(38273746)==0
end
function c38273745.tg1(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(1-tp) and chkc:IsOnField() and chkc:IsAbleToHand() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsAbleToHand,tp,0,LOCATION_ONFIELD,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local g=Duel.SelectTarget(tp,Card.IsAbleToHand,tp,0,LOCATION_ONFIELD,1,1,nil)
	e:GetHandler():RegisterFlagEffect(38273746,RESET_EVENT+0x1fe0000,EFFECT_FLAG_COPY_INHERIT,1)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,g:GetCount(),0,0)
end
function c38273745.op1(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
	end
end
function c38273745.con2(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetFlagEffect(38273747)==0
end
function c38273745.tg2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(1-tp,LOCATION_HAND,0)~=0 end
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,1-tp,LOCATION_HAND)
	e:GetHandler():RegisterFlagEffect(38273747,RESET_EVENT+0x1fe0000,EFFECT_FLAG_COPY_INHERIT,1)
end
function c38273745.op2(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetFieldGroup(1-tp,LOCATION_HAND,0)
	if g:GetCount()==0 then return end
	local sg=g:RandomSelect(1-tp,1)
	Duel.SendtoGrave(sg,REASON_EFFECT)
end
function c38273745.con3(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetFlagEffect(38273748)==0
end
function c38273745.tg3(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(1-tp) and chkc:IsLocation(LOCATION_GRAVE) and chkc:IsAbleToRemove() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsAbleToRemove,tp,0,LOCATION_GRAVE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectTarget(tp,Card.IsAbleToRemove,tp,0,LOCATION_GRAVE,1,1,nil)
	e:GetHandler():RegisterFlagEffect(38273748,RESET_EVENT+0x1fe0000,EFFECT_FLAG_COPY_INHERIT,1)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,g:GetCount(),0,0)
end
function c38273745.op3(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.Remove(tc,POS_FACEUP,REASON_EFFECT)
	end
end
