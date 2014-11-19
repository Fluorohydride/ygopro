--六武式三段衝
function c81426505.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c81426505.descon)
	e1:SetTarget(c81426505.destg)
	e1:SetOperation(c81426505.desop)
	c:RegisterEffect(e1)
end
function c81426505.confilter(c)
	return c:IsFaceup() and c:IsSetCard(0x3d)
end
function c81426505.descon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c81426505.confilter,tp,LOCATION_MZONE,0,3,nil)
end
function c81426505.filter1(c)
	return c:IsFaceup() and c:IsDestructable()
end
function c81426505.filter2(c)
	return c:IsFaceup() and c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsDestructable()
end
function c81426505.filter3(c)
	return c:IsFacedown() and c:IsDestructable()
end
function c81426505.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return 
		Duel.IsExistingMatchingCard(c81426505.filter1,tp,0,LOCATION_MZONE,1,nil) or
		Duel.IsExistingMatchingCard(c81426505.filter2,tp,0,LOCATION_ONFIELD,1,nil) or
		Duel.IsExistingMatchingCard(c81426505.filter3,tp,0,LOCATION_SZONE,1,nil)
	end
	local t={}
	local p=1
	if Duel.IsExistingMatchingCard(c81426505.filter1,tp,0,LOCATION_MZONE,1,nil) then t[p]=aux.Stringid(81426505,0) p=p+1 end
	if Duel.IsExistingMatchingCard(c81426505.filter2,tp,0,LOCATION_ONFIELD,1,nil) then t[p]=aux.Stringid(81426505,1) p=p+1 end
	if Duel.IsExistingMatchingCard(c81426505.filter3,tp,0,LOCATION_SZONE,1,nil) then t[p]=aux.Stringid(81426505,2) p=p+1 end
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(81426505,3))
	local sel=Duel.SelectOption(tp,table.unpack(t))+1
	local opt=t[sel]-aux.Stringid(81426505,0)
	local sg=nil
	if opt==0 then sg=Duel.GetMatchingGroup(c81426505.filter1,tp,0,LOCATION_MZONE,nil)
	elseif opt==1 then sg=Duel.GetMatchingGroup(c81426505.filter2,tp,0,LOCATION_ONFIELD,nil)
	else sg=Duel.GetMatchingGroup(c81426505.filter3,tp,0,LOCATION_SZONE,nil) end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,sg,sg:GetCount(),0,0)
	e:SetLabel(opt)
end
function c81426505.desop(e,tp,eg,ep,ev,re,r,rp)
	local opt=e:GetLabel()
	local sg=nil
	if opt==0 then sg=Duel.GetMatchingGroup(c81426505.filter1,tp,0,LOCATION_MZONE,nil)
	elseif opt==1 then sg=Duel.GetMatchingGroup(c81426505.filter2,tp,0,LOCATION_ONFIELD,nil)
	else sg=Duel.GetMatchingGroup(c81426505.filter3,tp,0,LOCATION_SZONE,nil) end
	Duel.Destroy(sg,REASON_EFFECT)
end
