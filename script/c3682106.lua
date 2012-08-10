--ダブルトラップ
function c3682106.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c3682106.target)
	e1:SetOperation(c3682106.activate)
	c:RegisterEffect(e1)
end
c3682106.collection={
	[40867519]=true;[67750322]=true;[73551138]=true;[01953925]=true;[65403020]=true;
	[77585513]=true;[35803249]=true;[94568601]=true;[74841885]=true;[66235877]=true;
	[19312169]=true;[51452091]=true;[92408984]=true;[62892347]=true;[42868711]=true;
	[90464188]=true;[03370104]=true;[88989706]=true;[20529766]=true;[74841885]=true;
	[53347303]=true;[06150044]=true;[49868263]=true;[51447164]=true;[44155002]=true;
	[74593218]=true;
}
function c3682106.filter(c)
	return c:IsFaceup() and c3682106.collection[c:GetCode()] and c:IsDestructable()
end
function c3682106.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and c3682106.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c3682106.filter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,e:GetHandler()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c3682106.filter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,1,e:GetHandler())
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c3682106.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
