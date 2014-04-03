--Ｔｈｅ　ｔｙｒａｎｔ　ＮＥＰＴＵＮＥ
function c88071625.initial_effect(c)
	--cannot special summon
	local e1=Effect.CreateEffect(c)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	e1:SetValue(aux.FALSE)
	c:RegisterEffect(e1)
	--summon/set with 1 tribute
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(88071625,0))
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_SUMMON_PROC)
	e2:SetCondition(c88071625.otcon)
	e2:SetOperation(c88071625.otop)
	e2:SetValue(SUMMON_TYPE_ADVANCE)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_SET_PROC)
	c:RegisterEffect(e3)
	--tribute check
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE)
	e4:SetCode(EFFECT_MATERIAL_CHECK)
	e4:SetValue(c88071625.valcheck)
	c:RegisterEffect(e4)
	--give atk effect only when  summon
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetCode(EFFECT_SUMMON_COST)
	e5:SetOperation(c88071625.facechk)
	e5:SetLabelObject(e4)
	c:RegisterEffect(e5)
	--copy
	local e6=Effect.CreateEffect(c)
	e6:SetDescription(aux.Stringid(88071625,1))
	e6:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e6:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e6:SetCode(EVENT_SUMMON_SUCCESS)
	e6:SetCondition(c88071625.copycon)
	e6:SetTarget(c88071625.copytg)
	e6:SetOperation(c88071625.copyop)
	c:RegisterEffect(e6)
end
function c88071625.otcon(e,c)
	if c==nil then return true end
	return c:GetLevel()>6 and Duel.GetTributeCount(c)>0
end
function c88071625.otop(e,tp,eg,ep,ev,re,r,rp,c)
	local sg=Duel.SelectTribute(tp,c,1,1)
	c:SetMaterial(sg)
	Duel.Release(sg, REASON_SUMMON+REASON_MATERIAL)
end
function c88071625.valcheck(e,c)
	local g=c:GetMaterial()
	local tc=g:GetFirst()
	local atk=0
	local def=0
	while tc do
		local catk=tc:GetTextAttack()
		local cdef=tc:GetTextDefence()
		atk=atk+(catk>=0 and catk or 0)
		def=def+(cdef>=0 and cdef or 0)
		tc=g:GetNext()
	end
	if e:GetLabel()==1 then
		e:SetLabel(0)
		--atk continuous effect
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
		e1:SetRange(LOCATION_MZONE)
		e1:SetValue(atk)
		e1:SetReset(RESET_EVENT+0xff0000)
		c:RegisterEffect(e1)
		--def continuous effect
		local e2=e1:Clone()
		e2:SetCode(EFFECT_UPDATE_DEFENCE)
		e2:SetValue(def)
		c:RegisterEffect(e2)
	end
end
function c88071625.facechk(e,tp,eg,ep,ev,re,r,rp)
	e:GetLabelObject():SetLabel(1)
end
function c88071625.copycon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_ADVANCE
end
function c88071625.filter(c,e)
	return c:IsType(TYPE_EFFECT) and c:IsLocation(LOCATION_GRAVE) and c:IsCanBeEffectTarget(e)
end
function c88071625.copytg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return e:GetHandler():GetMaterial():IsContains(chkc) and c88071625.filter(chkc,e) end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	local g=e:GetHandler():GetMaterial():FilterSelect(tp,c88071625.filter,1,1,nil,e)
	Duel.SetTargetCard(g)
end
function c88071625.copyop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) and c:IsRelateToEffect(e) and c:IsFaceup() then
		local code=tc:GetOriginalCode()
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetCode(EFFECT_CHANGE_CODE)
		e1:SetValue(code)
		c:RegisterEffect(e1)
		c:CopyEffect(code,RESET_EVENT+0x1fe0000,1)
	end
end
