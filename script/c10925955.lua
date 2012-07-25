--儀水鏡との交信
function c10925955.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c10925955.condition)
	e1:SetTarget(c10925955.target)
	e1:SetOperation(c10925955.activate)
	c:RegisterEffect(e1)
end
function c10925955.cfilter(c,rit)
	return c:IsFaceup() and c:IsAttribute(ATTRIBUTE_WATER) and (not rit or c:IsType(TYPE_RITUAL))
end
function c10925955.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c10925955.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c10925955.filter(c)
	return c:IsFacedown() and c:GetSequence()~=5
end
function c10925955.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)>1
		or Duel.GetFieldGroupCount(tp,0,LOCATION_DECK)>1
		or Duel.IsExistingMatchingCard(c10925955.filter,tp,0,LOCATION_SZONE,1,nil) end
	local sel=0
	local ac=0
	if Duel.IsExistingMatchingCard(c10925955.filter,tp,0,LOCATION_SZONE,1,nil) then sel=sel+1 end
	if Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)>1 or Duel.GetFieldGroupCount(tp,0,LOCATION_DECK)>1 then sel=sel+2 end
	if sel==1 then
		ac=Duel.SelectOption(tp,aux.Stringid(10925955,0))
	elseif sel==2 then
		ac=Duel.SelectOption(tp,aux.Stringid(10925955,1))+1
	elseif Duel.IsExistingMatchingCard(c10925955.cfilter,tp,LOCATION_MZONE,0,1,nil,true) then
		ac=Duel.SelectOption(tp,aux.Stringid(10925955,0),aux.Stringid(10925955,1),aux.Stringid(10925955,2))
	else
		ac=Duel.SelectOption(tp,aux.Stringid(10925955,0),aux.Stringid(10925955,1))
	end
	e:SetLabel(ac)
end
function c10925955.activate(e,tp,eg,ep,ev,re,r,rp)
	local ac=e:GetLabel()
	if ac==0 or ac==2 then
		local g=Duel.GetMatchingGroup(c10925955.filter,tp,0,LOCATION_SZONE,nil)
		Duel.ConfirmCards(tp,g)
	end
	if ac==1 or ac==2 then
		if Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)>1 and Duel.GetFieldGroupCount(tp,0,LOCATION_DECK)>1 then
			local st=Duel.SelectOption(tp,aux.Stringid(10925955,3),aux.Stringid(10925955,4))
			if st==0 then Duel.SortDecktop(tp,tp,2)
			else Duel.SortDecktop(tp,1-tp,2) end
		elseif Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)>1 then
			Duel.SortDecktop(tp,tp,2)
		elseif Duel.GetFieldGroupCount(tp,0,LOCATION_DECK)>1 then
			Duel.SortDecktop(tp,1-tp,2)
		end
	end
end
