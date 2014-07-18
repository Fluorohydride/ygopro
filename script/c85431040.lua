--イービル·ソーン
function c85431040.initial_effect(c)
	--damage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(85431040,0))
	e1:SetCategory(CATEGORY_DAMAGE+CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCost(c85431040.cost)
	e1:SetTarget(c85431040.target)
	e1:SetOperation(c85431040.operation)
	c:RegisterEffect(e1)
end
function c85431040.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c85431040.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,300)
end
function c85431040.filter(c,e,tp)
	return c:IsCode(85431040) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c85431040.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Damage(1-tp,300,REASON_EFFECT)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft<=0 then return end
	if ft>2 then ft=2 end
	local g=Duel.GetMatchingGroup(c85431040.filter,tp,LOCATION_DECK,0,nil,e,tp)
	if g:GetCount()==0 then return end
	if Duel.SelectYesNo(tp,aux.Stringid(85431040,1)) then
		--negate field card(tmp)
		local g0=Duel.GetFieldGroup(tp,LOCATION_ONFIELD,LOCATION_ONFIELD)
		local tmpc=g0:GetFirst()
		local nlist={}
		local ctr=1
		while tmpc do
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_DISABLE)
			e1:SetReset(RESET_CARD)
			tmpc:RegisterEffect(e1)
			nlist[ctr]=e1
			ctr=ctr+1
			tmpc=g0:GetNext()
		end
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local sg=g:Select(tp,1,ft,nil)
		local tc=sg:GetFirst()
		while tc do
			Duel.SpecialSummonStep(tc,0,tp,tp,false,false,POS_FACEUP_ATTACK)
			--cannot trigger
			local e2=Effect.CreateEffect(e:GetHandler())
			e2:SetType(EFFECT_TYPE_SINGLE)
			e2:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
			e2:SetCode(EFFECT_CANNOT_TRIGGER)
			e2:SetRange(LOCATION_MZONE)
			e2:SetReset(RESET_EVENT+0x1fe0000)
			tc:RegisterEffect(e2,true)
			tc=sg:GetNext()
		end
		--negate reset
		local i=0
		for i=1,ctr-1 do
			nlist[i]:Reset()
		end
		Duel.SpecialSummonComplete()
	end
end
