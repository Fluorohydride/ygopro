--機甲部隊の最前線
function c31828916.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetCode(EVENT_DESTROY)
	e2:SetRange(LOCATION_SZONE)
	e2:SetOperation(c31828916.check)
	c:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e3:SetDescription(aux.Stringid(31828916,0))
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EVENT_BATTLE_DESTROYED)
	e3:SetCountLimit(1)
	e3:SetTarget(c31828916.target)
	e3:SetOperation(c31828916.operation)
	e3:SetLabelObject(e2)
	c:RegisterEffect(e3)
end
function c31828916.check(e,tp,eg,ep,ev,re,r,rp)
	local pg=e:GetLabelObject()
	if pg then pg:DeleteGroup() end
	local dg=eg:Filter(Card.IsRace,nil,RACE_MACHINE)
	e:SetLabelObject(dg)
	dg:KeepAlive()
end
function c31828916.cfilter(c,cg,e,tp)
	return c:IsRace(RACE_MACHINE) and c:IsControler(tp) and cg:IsContains(c)
		and Duel.IsExistingMatchingCard(c31828916.filter,tp,LOCATION_DECK,0,1,nil,c:GetAttack(),c:GetAttribute(),e,tp)
end
function c31828916.filter(c,atk,att,e,tp)
	local a=c:GetAttack()
	return a>=0 and a<atk and c:IsRace(RACE_MACHINE) and c:IsAttribute(att)
		and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c31828916.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if Duel.GetLocationCount(tp,LOCATION_MZONE)==0 then return false end
		local dg=eg:Filter(c31828916.cfilter,nil,e:GetLabelObject():GetLabelObject(),e,tp)
		if dg:GetCount()==1 then
			e:SetLabel(1)
			e:SetLabelObject(dg:GetFirst())
			return true
		elseif dg:GetCount()==2 then
			e:SetLabel(2)
			return true
		end
		return false
	end
	if e:GetLabel()==2 then
		eg:GetFirst():CreateEffectRelation(e)
		eg:GetNext():CreateEffectRelation(e)
	else
		e:GetLabelObject():CreateEffectRelation(e)
	end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c31828916.operation(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)==0 then return end
	if e:GetLabel()==1 then
		local tc=e:GetLabelObject()
		if not tc:IsRelateToEffect(e) then return end
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local g=Duel.SelectMatchingCard(tp,c31828916.filter,tp,LOCATION_DECK,0,1,1,nil,tc:GetAttack(),tc:GetAttribute(),e,tp)
		if g:GetCount()>0 then
			Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
		end
	else
		local sg=eg:Filter(Card.IsRelateToEffect,nil,e)
		local tc=sg:GetFirst()
		if not tc then return end
		local atk=tc:GetAttack()
		local att=tc:GetAttribute()
		tc=sg:GetNext()
		if tc then
			if tc:GetAttack()>atk then atk=tc:GetAttack() end
			att=bit.bor(att,tc:GetAttribute())
		end
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local g=Duel.SelectMatchingCard(tp,c31828916.filter,tp,LOCATION_DECK,0,1,1,nil,atk,att,e,tp)
		if g:GetCount()>0 then
			Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
		end
	end
end
