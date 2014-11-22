--竜操術
function c99659159.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--atk
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_MZONE,0)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetTarget(c99659159.atktg)
	e2:SetValue(500)
	c:RegisterEffect(e2)
	--equip
	local e3=Effect.CreateEffect(c)
	e3:SetCategory(CATEGORY_EQUIP)
	e3:SetDescription(aux.Stringid(99659159,0))
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCountLimit(1)
	e3:SetTarget(c99659159.target)
	e3:SetOperation(c99659159.operation)
	c:RegisterEffect(e3)
end
function c99659159.atktg(e,c)
	return c:GetEquipGroup():IsExists(Card.IsSetCard,1,nil,0x29)
end
function c99659159.filter(c)
	return c:IsSetCard(0x29) and c:IsRace(RACE_DRAGON)
end
function c99659159.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_MZONE) and chkc:IsFaceup() end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_SZONE)>0
		and Duel.IsExistingTarget(Card.IsFaceup,tp,LOCATION_MZONE,0,1,nil)
		and Duel.IsExistingMatchingCard(c99659159.filter,tp,LOCATION_HAND,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,Card.IsFaceup,tp,LOCATION_MZONE,0,1,1,nil)
end
function c99659159.operation(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if Duel.GetLocationCount(tp,LOCATION_SZONE)<=0 then return end
	local tc=Duel.GetFirstTarget()
	if tc:IsFacedown() or not tc:IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
	local sg=Duel.SelectMatchingCard(tp,c99659159.filter,tp,LOCATION_HAND,0,1,1,nil)
	if sg:GetCount()==0 then return end
	local sc=sg:GetFirst()
	Duel.Equip(tp,sc,tc,true)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_EQUIP_LIMIT)
	e1:SetReset(RESET_EVENT+0x1fe0000)
	e1:SetValue(c99659159.eqlimit)
	e1:SetLabelObject(tc)
	sc:RegisterEffect(e1)
end
function c99659159.eqlimit(e,c)
	return e:GetLabelObject()==c
end
