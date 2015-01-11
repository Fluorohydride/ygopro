--突風
function c73079365.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c73079365.condition)
	e1:SetTarget(c73079365.target)
	e1:SetOperation(c73079365.activate)
	c:RegisterEffect(e1)
end
function c73079365.filter(c,tp)
	return c:IsType(TYPE_SPELL) and c:IsPreviousLocation(LOCATION_SZONE) and c:GetPreviousControler()==tp
		and bit.band(c:GetReason(),0x41)==0x41
end
function c73079365.condition(e,tp,eg,ep,ev,re,r,rp)
	return rp==1-tp and eg:IsExists(c73079365.filter,1,nil,tp)
end
function c73079365.desfilter(c)
	return c:IsDestructable() and c:IsType(TYPE_SPELL+TYPE_TRAP)
end
function c73079365.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and c73079365.desfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c73079365.desfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,e:GetHandler()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c73079365.desfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,1,e:GetHandler())
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c73079365.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
