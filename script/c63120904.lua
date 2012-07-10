--暗黒大要塞鯱
function c63120904.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(63120904,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTarget(c63120904.target)
	e1:SetOperation(c63120904.operation)
	c:RegisterEffect(e1)
end
function c63120904.rfilter(c)
	local code=c:GetCode()
	return code==90337190 or code==95614612
end
function c63120904.dfilter(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsDestructable()
end
function c63120904.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsDestructable() end
	if chk==0 then return (Duel.CheckReleaseGroup(tp,Card.IsCode,1,nil,90337190) and Duel.IsExistingTarget(Card.IsDestructable,tp,0,LOCATION_MZONE,1,nil))
		or (Duel.CheckReleaseGroup(tp,Card.IsCode,1,nil,95614612) and Duel.IsExistingTarget(c63120904.dfilter,tp,0,LOCATION_ONFIELD,1,nil)) end
	local b1=Duel.CheckReleaseGroup(tp,Card.IsCode,1,nil,90337190) and Duel.IsExistingTarget(Card.IsDestructable,tp,0,LOCATION_MZONE,1,nil)
	local b2=Duel.CheckReleaseGroup(tp,Card.IsCode,1,nil,95614612) and Duel.IsExistingTarget(c63120904.dfilter,tp,0,LOCATION_ONFIELD,1,nil)
	local code=0
	if b1 and b2 then
		local rg=Duel.SelectReleaseGroup(tp,c63120904.rfilter,1,1,nil)
		code=rg:GetFirst():GetCode()
		Duel.Release(rg,REASON_COST)
	elseif b1 then
		local rg=Duel.SelectReleaseGroup(tp,Card.IsCode,1,1,nil,90337190)
		code=90337190
		Duel.Release(rg,REASON_COST)
	else
		local rg=Duel.SelectReleaseGroup(tp,Card.IsCode,1,1,nil,95614612)
		code=95614612
		Duel.Release(rg,REASON_COST)
	end
	if code==90337190 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
		local g=Duel.SelectTarget(tp,Card.IsDestructable,tp,0,LOCATION_MZONE,1,1,nil)
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
		local g=Duel.SelectTarget(tp,c63120904.dfilter,tp,0,LOCATION_ONFIELD,1,1,nil)
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	end
end
function c63120904.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
