--セイクリッド・トレミスM7
function c38495396.initial_effect(c)
	--xyz summon
	c:EnableReviveLimit()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_EXTRA)
	e1:SetCondition(c38495396.xyzcon)
	e1:SetOperation(c38495396.xyzop)
	e1:SetValue(SUMMON_TYPE_XYZ)
	c:RegisterEffect(e1)
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
function c38495396.xyzfilter(c)
	return c:GetLevel()==6
end
function c38495396.ovfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x53) and c:GetCode()~=38495396 and c:IsType(TYPE_XYZ)
end
function c38495396.xyzcon(e,c)
	if c==nil then return true end
	local ft=Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)
	if ft<=-2 then return false end
	if ft>-1 and Duel.IsExistingMatchingCard(c38495396.ovfilter,c:GetControler(),LOCATION_MZONE,0,1,nil) then return true end
	local g=Duel.GetXyzMaterial(c)
	return g:IsExists(c38495396.xyzfilter,2,nil)
end
function c38495396.xyzop(e,tp,eg,ep,ev,re,r,rp,c)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	local g=Duel.GetXyzMaterial(c)
	local b1=ft>-2 and g:IsExists(c38495396.xyzfilter,2,nil)
	local b2=ft>-1 and Duel.IsExistingMatchingCard(c38495396.ovfilter,tp,LOCATION_MZONE,0,1,nil)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
	if (b1 and b2 and Duel.SelectYesNo(tp,aux.Stringid(38495396,1))) or ((not b1) and b2) then
		local mg=Duel.SelectMatchingCard(tp,c38495396.ovfilter,tp,LOCATION_MZONE,0,1,1,nil)
		local mg2=mg:GetFirst():GetOverlayGroup()
		Duel.Overlay(c,mg2)
		Duel.Overlay(c,mg)
		mg:Merge(mg2)
		c:SetMaterial(mg)
		c:RegisterFlagEffect(38495396,RESET_EVENT+0xfe0000+RESET_PHASE+PHASE_END,0,1)
	else
		local mg=g:FilterSelect(tp,c38495396.xyzfilter,2,2,nil)
		Duel.Overlay(c,mg)
		c:SetMaterial(mg)
	end
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
