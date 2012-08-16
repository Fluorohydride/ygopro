--パワーカプセル
function c54289683.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c54289683.target)
	e1:SetOperation(c54289683.operation)
	c:RegisterEffect(e1)
end
function c54289683.filter(c)
	return c:IsFaceup() and c:IsCode(93130021)
end
function c54289683.desfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsDestructable()
end
function c54289683.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return Duel.IsExistingTarget(c54289683.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	local cg=Duel.SelectTarget(tp,c54289683.filter,tp,LOCATION_MZONE,0,1,1,nil)
	local c=cg:GetFirst()
	local t1=Duel.IsExistingTarget(c54289683.desfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,e:GetHandler())
	local t2=Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsPlayerCanSpecialSummonMonster(tp,93130022,0,0x4011,c:GetAttack(),c:GetDefence(),c:GetLevel(),c:GetRace(),c:GetAttribute())
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(93130021,0))
	if t1 and t2 then
		op=Duel.SelectOption(tp,aux.Stringid(93130021,1),aux.Stringid(93130021,2),aux.Stringid(93130021,3))
	elseif t1 then
		op=Duel.SelectOption(tp,aux.Stringid(93130021,1),aux.Stringid(93130021,2))
	elseif t2 then
		op=Duel.SelectOption(tp,aux.Stringid(93130021,1),aux.Stringid(93130021,3))
		if op==1 then op=2 end
	else op=Duel.SelectOption(tp,aux.Stringid(93130021,1)) end
	e:SetLabel(op)
	if op==1 then
		e:SetCategory(CATEGORY_DESTROY)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
		local g=Duel.SelectTarget(tp,c54289683.desfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,1,e:GetHandler())
		e:SetLabelObject(g:GetFirst())
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	elseif op==2 then
		e:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_TOKEN)
		Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,0,0)
		Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,1,0,0)
	else
		e:SetCategory(CATEGORY_ATKCHANGE)
	end
end
function c54289683.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if e:GetLabel()==1 then
		local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
		local dc=e:GetLabelObject()
		tc=g:GetFirst()
		if tc==dc then tc=g:GetNext() end
		if tc:IsFaceup() and tc:IsRelateToEffect(e) and dc:IsRelateToEffect(e) then
			Duel.Destroy(dc,REASON_EFFECT)
		end
	elseif e:GetLabel()==2 then
		if tc:IsFacedown() or not tc:IsRelateToEffect(e) then return end
		local atk=tc:GetAttack()
		local def=tc:GetDefence()
		local lv=tc:GetLevel()
		local race=tc:GetRace()
		local att=tc:GetAttribute()
		if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0
			or not Duel.IsPlayerCanSpecialSummonMonster(tp,93130022,0,0x4011,atk,def,lv,race,att) then return end
		local token=Duel.CreateToken(tp,93130022)
		tc:CreateRelation(token,RESET_EVENT+0x1fe0000)
		Duel.SpecialSummonStep(token,0,tp,tp,false,false,POS_FACEUP)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_SET_ATTACK_FINAL)
		e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
		e1:SetRange(LOCATION_MZONE)
		e1:SetValue(c54289683.tokenatk)
		e1:SetLabelObject(tc)
		e1:SetReset(RESET_EVENT+0xfe0000)
		token:RegisterEffect(e1,true)
		local e2=e1:Clone()
		e2:SetCode(EFFECT_SET_DEFENCE_FINAL)
		e2:SetValue(c54289683.tokendef)
		e2:SetLabelObject(tc)
		token:RegisterEffect(e2,true)
		local e3=Effect.CreateEffect(c)
		e3:SetType(EFFECT_TYPE_SINGLE)
		e3:SetCode(EFFECT_CHANGE_LEVEL)
		e3:SetValue(c54289683.tokenlv)
		e3:SetLabelObject(tc)
		e3:SetReset(RESET_EVENT+0xfe0000)
		token:RegisterEffect(e3,true)
		local e4=Effect.CreateEffect(c)
		e4:SetType(EFFECT_TYPE_SINGLE)
		e4:SetCode(EFFECT_CHANGE_RACE)
		e4:SetValue(c54289683.tokenrace)
		e4:SetLabelObject(tc)
		e4:SetReset(RESET_EVENT+0xfe0000)
		token:RegisterEffect(e4,true)
		local e5=Effect.CreateEffect(c)
		e5:SetType(EFFECT_TYPE_SINGLE)
		e5:SetCode(EFFECT_CHANGE_ATTRIBUTE)
		e5:SetValue(c54289683.tokenatt)
		e5:SetLabelObject(tc)
		e5:SetReset(RESET_EVENT+0xfe0000)
		token:RegisterEffect(e5,true)
		local e6=Effect.CreateEffect(c)
		e6:SetType(EFFECT_TYPE_SINGLE)
		e6:SetCode(EFFECT_SELF_DESTROY)
		e6:SetCondition(c54289683.tokendes)
		e6:SetLabelObject(tc)
		e6:SetReset(RESET_EVENT+0xfe0000)
		token:RegisterEffect(e6,true)
		Duel.SpecialSummonComplete()
	else
		if tc:IsFacedown() or not tc:IsRelateToEffect(e) then return end
		if tc:IsRelateToEffect(e) and tc:IsFaceup() then
			local e1=Effect.CreateEffect(c)
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_UPDATE_ATTACK)
			e1:SetValue(400)
			e1:SetReset(RESET_EVENT+0x1ff0000)
			tc:RegisterEffect(e1)
		end
	end
end
function c54289683.tokenatk(e,c)
	return e:GetLabelObject():GetAttack()
end
function c54289683.tokendef(e,c)
	return e:GetLabelObject():GetDefence()
end
function c54289683.tokenlv(e,c)
	return e:GetLabelObject():GetLevel()
end
function c54289683.tokenrace(e,c)
	return e:GetLabelObject():GetRace()
end
function c54289683.tokenatt(e,c)
	return e:GetLabelObject():GetAttribute()
end
function c54289683.tokendes(e)
	return not e:GetLabelObject():IsRelateToCard(e:GetHandler())
end
