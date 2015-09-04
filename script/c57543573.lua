--ワーム・イリダン
function c57543573.initial_effect(c)
	--add counter
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_MSET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c57543573.accon1)
	e1:SetOperation(c57543573.acop)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_SSET)
	c:RegisterEffect(e2)
	local e3=e1:Clone()
	e3:SetCode(EVENT_CHANGE_POS)
	e3:SetCondition(c57543573.accon2)
	c:RegisterEffect(e3)
	local e4=e1:Clone()
	e4:SetCode(EVENT_SPSUMMON_SUCCESS)
	e4:SetCondition(c57543573.accon3)
	c:RegisterEffect(e4)
	--destroy
	local e5=Effect.CreateEffect(c)
	e5:SetDescription(aux.Stringid(57543573,0))
	e5:SetCategory(CATEGORY_DESTROY)
	e5:SetType(EFFECT_TYPE_IGNITION)
	e5:SetRange(LOCATION_MZONE)
	e5:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e5:SetCost(c57543573.descost)
	e5:SetTarget(c57543573.destg)
	e5:SetOperation(c57543573.desop)
	c:RegisterEffect(e5)
end
function c57543573.accon1(e,tp,eg,ep,ev,re,r,rp)
	return eg:GetFirst():IsControler(tp)
end
function c57543573.filter2(c,tp)
	return c:IsControler(tp) and bit.band(c:GetPreviousPosition(),POS_FACEUP)~=0 and bit.band(c:GetPosition(),POS_FACEDOWN)~=0
end
function c57543573.accon2(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c57543573.filter2,1,nil,tp)
end
function c57543573.filter3(c,tp)
	return c:IsControler(tp) and c:IsFacedown()
end
function c57543573.accon3(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c57543573.filter3,1,nil,tp)
end
function c57543573.acop(e,tp,eg,ep,ev,re,r,rp)
	e:GetHandler():AddCounter(0xf,1)
end
function c57543573.descost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsCanRemoveCounter(tp,0xf,2,REASON_COST) end
	e:GetHandler():RemoveCounter(tp,0xf,2,REASON_COST)
end
function c57543573.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsControler(1-tp) and chkc:IsDestructable() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsDestructable,tp,0,LOCATION_ONFIELD,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,Card.IsDestructable,tp,0,LOCATION_ONFIELD,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c57543573.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
