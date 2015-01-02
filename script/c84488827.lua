--インヴェルズ・ガザス
function c84488827.initial_effect(c)
	--summon success
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(84488827,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetCondition(c84488827.condition)
	e1:SetTarget(c84488827.target)
	e1:SetOperation(c84488827.operation)
	c:RegisterEffect(e1)
	--tribute check
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_MATERIAL_CHECK)
	e2:SetValue(c84488827.valcheck)
	e2:SetLabelObject(e1)
	c:RegisterEffect(e2)
end
function c84488827.valcheck(e,c)
	local g=c:GetMaterial()
	if g:IsExists(Card.IsSetCard,2,nil,0x100a) or g:IsExists(Card.IsCode,1,nil,62729173) then
		e:GetLabelObject():SetLabel(1)
	else
		e:GetLabelObject():SetLabel(0)
	end
end
function c84488827.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_ADVANCE and e:GetLabel()==1
end
function c84488827.sfilter(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsDestructable()
end
function c84488827.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local op=0
	local g1=Duel.GetMatchingGroup(Card.IsDestructable,tp,LOCATION_MZONE,LOCATION_MZONE,e:GetHandler())
	local g2=Duel.GetMatchingGroup(c84488827.sfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(84488827,0))
	if g1:GetCount()>0 and g2:GetCount()>0 then
		op=Duel.SelectOption(tp,aux.Stringid(84488827,1),aux.Stringid(84488827,2))+1
	elseif g1:GetCount()>0 then
		op=Duel.SelectOption(tp,aux.Stringid(84488827,1))+1
	elseif g2:GetCount()>0 then
		op=Duel.SelectOption(tp,aux.Stringid(84488827,2))+2
	end
	if op==1 then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g1,g1:GetCount(),0,0)
	elseif op==2 then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g2,g2:GetCount(),0,0)
	end
	e:SetLabel(op)
end
function c84488827.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==1 then
		local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,LOCATION_MZONE,LOCATION_MZONE,e:GetHandler())
		Duel.Destroy(g,REASON_EFFECT)
	elseif e:GetLabel()==2 then
		local g=Duel.GetMatchingGroup(c84488827.sfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
		Duel.Destroy(g,REASON_EFFECT)
	end
end
